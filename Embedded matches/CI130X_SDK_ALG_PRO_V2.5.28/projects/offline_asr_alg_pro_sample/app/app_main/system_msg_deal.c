/**
 * @file system_msg_deal.c
 * @brief  系统消息处理任务
 * @version V1.0.0
 * @date 2019.01.22
 *
 * @copyright Copyright (c) 2019  Chipintelli Technology Co., Ltd.
 *
 */
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "system_msg_deal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"
#include "ci_log.h"
#include "audio_play_api.h"
#include "sed_app_host.h"
#include "cias_network_msg_protocol.h"
#include "cias_network_msg_send_task.h"
#include "cias_voice_upload.h"
#include "sdk_default_config.h"
#include "ci130x_iisdma.h"
#include "ci130x_iis.h"
#include "ci130x_lowpower.h"
#include "ci130x_core_misc.h"
// #include "ci130x_audio_capture.h"
#include "voice_module_uart_protocol.h"
#include "prompt_player.h"
#include "product_semantic.h"
#include "ci_nvdata_manage.h"
#include "asr_api.h"
#include "user_msg_deal.h"
#include "system_hook.h"
#include "status_share.h"
#include "codec_manager.h"
#include "remote_api_for_host.h"

#include "cwsl_manage.h"
#if USE_IR_ENABEL
#include "main_device.h"
#include "middle_device.h"
#include "ir_uart_msg_deal.h"
#include "buzzer_voice.h"
#endif
/* 系统消息处理任务状态 */
typedef enum
{
    USERSTATE_WAIT_MSG = 0,    /* 等待消息 */
    SYS_STATE_WAKUP_TIMEOUT,   /* 消息超时 */
}user_task_state_t;

/* 系统状态结构 */
struct sys_manage_type
{
    uint8_t user_msg_state;             /* 系统消息处理任务状态                     */
    sys_wakeup_state_t wakeup_state;    /* 系统状态         0:非唤醒状态 1:唤醒状态 */
    sys_asr_state_t asr_state;          /* asr状态          0:空闲       1:忙碌     */
    uint8_t volset;                     /* 音量设置                                 */
    uint8_t pause_asr_count;            /* 用于保证语音识别开关配对                 */
    #if USE_AEC_MODULE
    TimerHandle_t intercept_timer_handle;
    #endif
}sys_manage_data = {
    .wakeup_state = SYS_STATE_UNWAKEUP,
    .user_msg_state = USERSTATE_WAIT_MSG,
    .pause_asr_count = 0,
    .volset = VOLUME_MAX + 1,
    #if USE_AEC_MODULE
    .intercept_timer_handle = 0,
    #endif
};


/* 唤醒互斥锁 */
SemaphoreHandle_t WakeupMutex = NULL;

/* 退出唤醒定时器 */
xTimerHandle exit_wakeup_timer = NULL;

/* 系统消息队列 */
static QueueHandle_t sys_msg_queue = NULL;
/* 用于忽略退出唤醒的标志，因为存在收到asr识别结果的同时收到退出唤醒的定时器事件，此时不应退出唤醒 */
static int8_t ignore_exit_wakeup = 0;

/*用于忽略语音识别消息 */
static int8_t ignore_asr_msg = 0;


#if USE_IR_ENABEL
static int exit_wakeup_timeout = EXIT_WAKEUP_TIME;

void change_wakeup_period(uint32_t exit_wakup_ms)
{
    exit_wakeup_timeout = exit_wakup_ms;
}
#endif
/**
 * @brief check now playing state, AEC or DENOISE maybe used this function
 *
 * @return true now is playing or ready to playing
 * @return false now not playing or ready to stop play
 */
bool check_current_playing(void)
{
    if(AUDIO_PLAY_STATE_IDLE != get_audio_play_state())
    {
        return true;
    }
    else
    {
        return false;
    }
}


/**
 * @brief 这个接口已经被弃用，没有实际功能，后续会去掉，请不要再使用。
 *
 */
void pause_voice_in(void)
{

}

/**
 * @brief 这个接口已经被弃用，没有实际功能，后续会去掉，请不要再使用。
 *
 */
void  resume_voice_in(void)
{
}


#if USE_AEC_MODULE
void intercept_timer_callback(TimerHandle_t xTimer)
{
    ciss_set(CI_SS_INTERCEPT_ASR_OUT, 0);
    xTimerStop(sys_manage_data.intercept_timer_handle, 0);
} 
#endif


/**
 * @brief 暂停语音识别。
 * @param voice_in_mute 是否mute录音,1:mute,0:不mute.
 * @param pause_asr_task 是否暂停识别任务,1:暂停, 0:不暂停.
 */
void pause_asr(uint8_t voice_in_mute, uint8_t pause_asr_task)
{
#if !NO_ASR_FLOW
    if (voice_in_mute && (ciss_get(CI_SS_MIC_VOICE_STATUE) != CI_SS_MIC_VOICE_MUTE))
    {
        cm_set_codec_mute(HOST_MIC_RECORD_CODEC_ID,CODEC_INPUT,1,ENABLE);
        ciss_set(CI_SS_MIC_VOICE_STATUE,CI_SS_MIC_VOICE_MUTE);
    }
    if (pause_asr_task && (!ciss_get(CI_SS_ASR_TASK_PAUSED)))
    {
        asrtop_asr_system_pause();
        ciss_set(CI_SS_ASR_TASK_PAUSED, 1);
    }
    
    #if USE_AEC_MODULE
    ciss_set(CI_SS_INTERCEPT_ASR_OUT, 1);
    #endif

    sys_manage_data.pause_asr_count++;
#endif
#if USE_IR_ENABEL
      xTimerStop(exit_wakeup_timer,0);
#endif
}

/**
 * @brief 恢复语音识别。
 *
 */
void  resume_asr()
{
#if !NO_ASR_FLOW
    if (sys_manage_data.pause_asr_count > 0)
    {
        sys_manage_data.pause_asr_count--;
        if (sys_manage_data.pause_asr_count == 0)
        {
            if (CI_SS_MIC_VOICE_MUTE == ciss_get(CI_SS_MIC_VOICE_STATUE))
            {
                cm_set_codec_mute(HOST_MIC_RECORD_CODEC_ID,CODEC_INPUT,1,DISABLE);
                ciss_set(CI_SS_MIC_VOICE_STATUE,CI_SS_MIC_VOICE_NORMAL);
            }
            if (ciss_get(CI_SS_ASR_TASK_PAUSED))
            {
                asrtop_asr_system_continue();
                ciss_set(CI_SS_ASR_TASK_PAUSED, 0);
            }

            #if USE_AEC_MODULE
            if (sys_manage_data.intercept_timer_handle == NULL)
            {
                sys_manage_data.intercept_timer_handle = xTimerCreate("intercept_timer", pdMS_TO_TICKS(500),pdFALSE, (void *)0, intercept_timer_callback);
            }
            xTimerStart(sys_manage_data.intercept_timer_handle, 0);
            #endif
        }
    }
#endif
    #if USE_IR_ENABEL
    if (sys_manage_data.wakeup_state == SYS_STATE_WAKEUP)
    {
        xTimerChangePeriod(exit_wakeup_timer, pdMS_TO_TICKS(exit_wakeup_timeout),0);/*or used a new timer*/
        xTimerStart(exit_wakeup_timer, 0);
    }
    #endif
}


/**
 * @brief 命令词识别结果播报完成回调函数
 *
 * @param cmd_handle 命令信息句柄
 */
void default_play_done_callback(cmd_handle_t cmd_handle)
{
}


/**
 * @brief Get the wakeup state object
 *
 * @return sys_wakeup_state_t
 */
sys_wakeup_state_t get_wakeup_state(void)
{
    return sys_manage_data.wakeup_state;
}


/**
 * @brief Get the asr state
 *
 * @return sys_asr_state_t asr状态
 */
sys_asr_state_t get_asr_state(void)
{
    return sys_manage_data.asr_state;
}


/**
 * @brief 设置状态为唤醒
 *
 * @param exit_wakup_ms 下次退出唤醒时间，单位ms
 */
void set_state_enter_wakeup(uint32_t exit_wakup_ms)
{
    sys_manage_data.wakeup_state = SYS_STATE_WAKEUP;/*update wakeup state*/
    ciss_set(CI_SS_WAKING_UP_STATE,CI_SS_WAKEUPED);
    ciss_set(CI_SS_WAKING_UP_STATE_FOR_SSP,CI_SS_WAKEUPED);
    xTimerStop(exit_wakeup_timer,0);
    xTimerChangePeriod(exit_wakeup_timer,pdMS_TO_TICKS(exit_wakup_ms),0);/*or used a new timer*/
    xTimerStart(exit_wakeup_timer,0);
}


/**
 * @brief 更新唤醒超时时间，保持唤醒状态
 *
 */
void update_awake_time(void)
{
    if (sys_manage_data.wakeup_state == SYS_STATE_WAKEUP)
    {
        set_state_enter_wakeup(EXIT_WAKEUP_TIME);
    }
}


/**
 * @brief 设置状态为退出唤醒
 *
 */
void set_state_exit_wakeup(void)
{
#if USE_CWSL   //自学习
    extern int cwsl_app_reset();
    #if USE_AEC_MODULE
    ciss_set(CI_SS_CWSL_AEC_MUTE_STATE,CI_SS_CWSL_AEC_MUTE_OFF);
    #endif
    cwsl_app_reset();
#endif
    xTimerStop(exit_wakeup_timer,0);
    sys_manage_data.wakeup_state = SYS_STATE_UNWAKEUP;
    ciss_set(CI_SS_WAKING_UP_STATE,CI_SS_NO_WAKEUP);
    ciss_set(CI_SS_WAKING_UP_STATE_FOR_SSP,CI_SS_NO_WAKEUP);
    ciss_set(CI_SS_CMD_STATE,CI_SS_CMD_IS_NULL);
    ciss_set(CI_SS_CMD_STATE_FOR_SSP,CI_SS_CMD_IS_NULL);
}


#if USE_LOWPOWER_OSC_FREQUENCY
/* 退出降频模式进入osc时钟模式定时器 */
xTimerHandle exit_down_freq_mode_timer = NULL;

/**
 * @brief 退出降频模式进入osc时钟模式定时器
 *
 * @param xTimer 定时器句柄
 */
void exit_down_freq_mode_cb(TimerHandle_t xTimer)
{
    xTimerStop(exit_down_freq_mode_timer,0);

    xSemaphoreTake(WakeupMutex, portMAX_DELAY);

    if((SYS_STATE_UNWAKEUP == get_wakeup_state())&&(POWER_MODE_NORMAL == get_curr_power_mode()))
    {
        if(( 0 == asrtop_sys_isbusy())&&(AUDIO_PLAY_STATE_IDLE == get_audio_play_state()))
        {
            pause_asr(0, 1);
            #if (USE_DENOISE_MODULE)
            power_mode_switch(POWER_MODE_DOWN_FREQUENCY);
            #else
            power_mode_switch(POWER_MODE_OSC_FREQUENCY);
            #endif
            resume_asr();
        }
        else
        {
            xTimerStart(exit_down_freq_mode_timer,0);
        }
    }
    xSemaphoreGive(WakeupMutex);
}
#endif

#if USE_LOWPOWER_OSC_FREQUENCY
/**
 * @brief vad start中断回调函数，这个函数在vad start中断中调用，
 *          此时需要将频率提升降频模式以上方可保证识别流程正常进行
 */
void vad_start_irq_cb(void)
{
    /* 如果系统处于晶振频率模式，在vad start时切换到正常模式运行 */
    if(POWER_MODE_OSC_FREQUENCY == get_curr_power_mode())
    {
        // cm_stop_codec(HOST_MIC_RECORD_CODEC_ID, CODEC_INPUT);
        // audio_pre_rslt_stop();
        power_mode_switch(POWER_MODE_NORMAL);
        // cm_start_codec(HOST_MIC_RECORD_CODEC_ID, CODEC_INPUT);
        // audio_pre_rslt_start();
        xTimerStartFromISR(exit_down_freq_mode_timer,0);
    }
}
#endif



/**
 * @brief 切换唤醒模型，这个函数是sys msg任务调用，其他任务需要切换模型需要发送切换模型消息
 *          通过sys msg任务调用
 */
void change_asr_wakeup_word(void)
{
    xSemaphoreTake(WakeupMutex, portMAX_DELAY);

    /*set wakeup state*/
    set_state_exit_wakeup();

    sys_sleep_hook();
    
    xSemaphoreGive(WakeupMutex);

    #if ADAPTIVE_THRESHOLD
    dynmic_confidence_en_cfg(0);
    #endif
    #if DEEP_SEPARATE_ENABLE && !BF_DEEPSE_MODE 
    mprintf("set CI_SS_CMD_ASR_CHA_NUM 2\n");
    ciss_set(CI_SS_CMD_ASR_CHA_NUM, 2);      
    #endif
    #if USE_SEPARATE_WAKEUP_EN
    cmd_info_change_cur_model_group(1);
    ignore_asr_msg++;

    sys_msg_t send_msg;
    send_msg.msg_type = SYS_MSG_TYPE_CMD_INFO;
    send_msg.msg_data.cmd_info_data.cmd_info_status = MSG_CMD_INFO_STATUS_ENABLE_PROCESS_ASR;
    send_msg_to_sys_task(&send_msg, NULL);
    #endif

    #if (ASR_SKIP_FRAME_CONFIG == 1)
    //if (get_cur_lm_states() < CLOSE_SKIP_MAX_MODEL_SIZE)
    {
        asr_dynamic_skip_close();
    }
    #endif

    #if USE_LOWPOWER_OSC_FREQUENCY
    // 晶振时钟模式，可以产生VAD START，不能识别需要在VAD START中断内切换到正常模式才可以识别
    pause_asr(0, 1);

    #if (USE_DENOISE_MODULE)
    power_mode_switch(POWER_MODE_DOWN_FREQUENCY);
    #else
    power_mode_switch(POWER_MODE_OSC_FREQUENCY);
    #endif

    resume_asr();
    #elif USE_LOWPOWER_DOWN_FREQUENCY
    // 降频模式，可以进行正常识别，在进入唤醒模式后切换回正常模式即可
    pause_asr(0, 1);
    power_mode_switch(POWER_MODE_DOWN_FREQUENCY);
    resume_asr();
    #endif
}


/**
 * @brief 切换正常模型，这个函数是sys msg任务调用，其他任务需要切换模型需要发送切换模型消息
 *          通过sys msg任务调用
 *
 */
void change_asr_normal_word(void)
{
    #if (ASR_SKIP_FRAME_CONFIG == 1)
    asr_dynamic_skip_open();
    #endif
    #if DEEP_SEPARATE_ENABLE && !BF_DEEPSE_MODE 
    mprintf("set CI_SS_CMD_ASR_CHA_NUM 1\n");
    ciss_set(CI_SS_CMD_ASR_CHA_NUM,1);
    #endif
    #if USE_SEPARATE_WAKEUP_EN
    cmd_info_change_cur_model_group(0);
    ignore_asr_msg++;

    sys_msg_t send_msg;
    send_msg.msg_type = SYS_MSG_TYPE_CMD_INFO;
    send_msg.msg_data.cmd_info_data.cmd_info_status = MSG_CMD_INFO_STATUS_ENABLE_PROCESS_ASR;
    send_msg_to_sys_task(&send_msg, NULL);
    #endif

    #if ADAPTIVE_THRESHOLD
    dynmic_confidence_en_cfg(1);
    dynmic_confidence_config(-20, 10, 1);
    #endif
    #if USE_CWSL
    ciss_set(CI_SS_START_SLEEP_PROCESS, 0);
    #endif
}


/**
 * @brief 进入唤醒模式播放完毕回调函数
 *          相比正常的播放完毕回调函数，除了要解除mute之外需要发送切换模型的消息以切换到正常模型
 *
 * @param cmd_handle
 */
void play_enter_wakeup_done_cb(cmd_handle_t cmd_handle)
{
    sys_msg_t send_msg;
    send_msg.msg_type = SYS_MSG_TYPE_CMD_INFO;
    send_msg.msg_data.cmd_info_data.cmd_info_status = MSG_CMD_INFO_STATUS_POST_CHANGE_ASR_NORMAL_WORD;
    send_msg_to_sys_task(&send_msg, NULL);
}


/**
 * @brief 退出唤醒模式播放完毕回调函数
 *          相比正常的播放完毕回调函数，除了要解除mute之外需要发送切换模型的消息以切换到唤醒模型，并设置系统状态
 *
 * @param cmd_handle
 */
void play_exit_wakeup_done_cb(cmd_handle_t cmd_handle)
{
    sys_msg_t send_msg;
    send_msg.msg_type = SYS_MSG_TYPE_CMD_INFO;
    send_msg.msg_data.cmd_info_data.cmd_info_status = MSG_CMD_INFO_STATUS_POST_CHANGE_ASR_WAKEUP_WORD;
    send_msg_to_sys_task(&send_msg, NULL);
    mprintf("inactivate\n");
    #if USE_IR_ENABEL
    user_led_light_flash();
    #endif

}


/**
 * @brief when exit wakup state, need deal some common thing, such as
 *        power mode set, change asr word, wakup state flag set, play wakeup prompt.
 *        asr_busy_check used for immediately exit or wait for current ASR deal done.
 *
 * @param asr_busy_check : 0:no need check asr busy, 1:need check asr busy
 */
void exit_wakeup_deal(uint32_t asr_busy_check)
{
    xSemaphoreTake(WakeupMutex, portMAX_DELAY);

    /*already in unwakeup state, so do nothing*/
    if(SYS_STATE_UNWAKEUP == get_wakeup_state())
    {
        xSemaphoreGive(WakeupMutex);
        return;
    }

    /*now asr busy,so need wait not busy, then call this function again*/
    if((1 == asr_busy_check)&&(SYS_STATE_ASR_BUSY == get_asr_state()))
    {
        sys_manage_data.user_msg_state = SYS_STATE_WAKUP_TIMEOUT;
        xSemaphoreGive(WakeupMutex);
        return;
    }
    #if PLAY_EXIT_WAKEUP_EN
    /*play exit wakeup voice*/
    prompt_play_by_cmd_string("<inactivate>", -1, play_exit_wakeup_done_cb,false);
    #else
    play_exit_wakeup_done_cb(0);
    #endif
    #if USE_VPR
    extern bool vpr_delete_tmp_flag;
    extern bool vpr_delete_all_flag;
    extern int8_t g_voice_print_rec_index;
    vpr_delete_tmp_flag = 0;
    vpr_delete_all_flag = 0;
    g_voice_print_rec_index = -1;
    #endif
    #if USE_IR_ENABEL
    main_device_overLearnIR(pdFALSE, 0);
    #endif
    xSemaphoreGive(WakeupMutex);
}


/**
 * @brief when enter wakup state, need deal some common thing, such as
 *        power mode set, change asr word, wakup state flag set, play wakeup prompt
 *
 * @param exit_wakup_ms : wakeup state keep times,
 */
void enter_wakeup_deal(uint32_t exit_wakup_ms, cmd_handle_t cmd_handle )
{
    xSemaphoreTake(WakeupMutex, portMAX_DELAY);

    /*if last state is unwakeup, need change asr word*/
    if(SYS_STATE_UNWAKEUP == get_wakeup_state())
    {
        #if (USE_LOWPOWER_OSC_FREQUENCY||USE_LOWPOWER_DOWN_FREQUENCY)
        pause_asr(0, 1);
        UartPollingSenddone((UART_TypeDef*)CONFIG_CI_LOG_UART);
        power_mode_switch(POWER_MODE_NORMAL);
        resume_asr();
        #endif
    }

    //if(cmd_handle != INVALID_HANDLE)
    #if (COMMAND_INFO_VER == 3)
    if (cmd_info_is_auto_play(cmd_handle))
    #endif
    {
        /*if last state is unwakeup, need change asr word*/
        #if USE_VPR
        uint8_t vpr_status = (uint8_t)vpr_get_status();
        if(vpr_status == 2)
        #endif
        {
            #if USE_CWSL
            if(SYS_STATE_UNWAKEUP == get_wakeup_state() || ciss_get(CI_SS_START_SLEEP_PROCESS) == 1)    //AEC打断退出唤醒播报时,CI_SS_START_SLEEP_PROCESS要置0
            #else
            if(SYS_STATE_UNWAKEUP == get_wakeup_state())
            #endif
            {
                #if PLAY_ENTER_WAKEUP_EN && !WMAN_PLAY_EN
                prompt_play_by_cmd_handle(cmd_handle, -1, play_enter_wakeup_done_cb,true);
                #else
                play_enter_wakeup_done_cb(cmd_handle);
                #endif
            }
            else
            {
                #if PLAY_ENTER_WAKEUP_EN && !WMAN_PLAY_EN
                prompt_play_by_cmd_handle(cmd_handle, -1, default_play_done_callback,true);
                #else
                default_play_done_callback(cmd_handle);
                #endif
            }
        }

        #if USE_VPR
            if(vpr_status != 2)
            {
                play_enter_wakeup_done_cb(cmd_handle);
            }
        #endif

        #if USE_IR_ENABEL
        if (NULL != DeviceGetLearnModeDevice())
        {
            //有匹配的时候 退出匹配状态
            main_device_overLearnIR(pdFALSE, 0);
        }
        #endif
    }
    /*set wakeup state,and update timer*/
    set_state_enter_wakeup(exit_wakup_ms);
	sys_weakup_hook();

    xSemaphoreGive(WakeupMutex);
	#if USE_VPR  || USE_WMAN_VPR
	vpr_run_one_recognition();
	#endif
}


/**
 * @brief : exit wakup timer callback function, this sample code will wait asr idle when exit wakeup,
 *          if you want immediately eixt wakeup, use exit_wakeup_deal(0)
 *
 * @param xTimer : timer handle
 */
void exit_wakeup_timer_callback(TimerHandle_t xTimer)
{
    #if USE_CWSL
    ciss_set(CI_SS_START_SLEEP_PROCESS, 1);
    #endif
    (void)xTimer;
    sys_msg_t send_msg;
    send_msg.msg_type = SYS_MSG_TYPE_CMD_INFO;
    send_msg.msg_data.cmd_info_data.cmd_info_status = MSG_CMD_INFO_STATUS_EXIT_WAKEUP;
    send_msg_to_sys_task(&send_msg, NULL);
}


/**
 * @brief 音量设置函数
 *
 * @param vol 音量值
 */
uint8_t vol_set(char vol)
{
    if(vol <= VOLUME_MAX && vol >= VOLUME_MIN && sys_manage_data.volset != vol)
    {
        sys_manage_data.volset = vol;
        audio_play_set_vol_gain(67*vol/VOLUME_MAX + 7);
        cinv_item_write(NVDATA_ID_VOLUME, sizeof(sys_manage_data.volset), &sys_manage_data.volset);
    }
    return sys_manage_data.volset;
}

uint8_t vol_get(void)
{
    return sys_manage_data.volset;
}

/**
 * @brief 系统消息任务资源初始化
 *
 */
void sys_msg_task_initial(void)
{
    sys_msg_queue = xQueueCreate(16, sizeof(sys_msg_t));
    if(!sys_msg_queue)
    {
        mprintf("not enough memory:%d,%s\n",__LINE__,__FUNCTION__);
    }

    WakeupMutex = xSemaphoreCreateMutex();
    if(!WakeupMutex)
    {
        mprintf("not enough memory:%d,%s\n",__LINE__,__FUNCTION__);
    }
}


/**
 * @brief A simple code for other component send system message to this module, just wrap freertos queue function
 *
 * @param flag_from_isr : 0 call this function not from isr, other call this from isr
 * @param send_msg : system message
 * @param xHigherPriorityTaskWoken : if call this not from isr set NULL
 * @return BaseType_t
 */
BaseType_t send_msg_to_sys_task(sys_msg_t *send_msg,BaseType_t *xHigherPriorityTaskWoken)
{
    if(0 != check_curr_trap())
    {
        return xQueueSendFromISR(sys_msg_queue,send_msg,xHigherPriorityTaskWoken);
    }
    else
    {
        return xQueueSend(sys_msg_queue,send_msg,0);
    }
}


/**
 * @brief 处理一些切换模型请求
 *
 * @param cmd_info_msg
 */
void sys_deal_cmd_info_msg(sys_msg_cmd_info_data_t *cmd_info_msg)
{
    /**
     * 这个函数处理退出唤醒模式和切换模型的请求，这些请求的发起方往往是其他线程，切模型和播放如果是异步的
     * 将导致播放获取的固件信息丢失而产生播放错误，所以播放请求和切模型的流程全部在sys_msg线程中执行将不会
     * 出现这个问题。
     * 关于理解ignore_exit_wakeup时，请切记播放是一个异步的请求，在播放到播放完毕期间是可能产生新的系统状态
     * 更新请求，并切换模型，低功耗的流程。
     */
    if(MSG_CMD_INFO_STATUS_POST_CHANGE_ASR_WAKEUP_WORD == cmd_info_msg->cmd_info_status)
    {
        /**
         * 切换到唤醒网络请求，这个请求是在播放完退出唤醒提示音后发送的消息，这里要判断ignore_exit_wakeup变量
         * 是由于在退出唤醒播放的过程中可能出现再次唤醒，这时候再发送切换模型的消息应该将其丢弃，系统状态已经
         * 被新的识别消息重置。切换完毕模型之后将进入低功耗策略，也就是切模型本身是工作于非低功耗下。
         */
        if(ignore_exit_wakeup == 0)
        {
            /*change asr wakeup word*/
            change_asr_wakeup_word();
        }
    }
    else if(MSG_CMD_INFO_STATUS_POST_CHANGE_ASR_NORMAL_WORD == cmd_info_msg->cmd_info_status)
    {
        /* 切换到正常的命令词网络，这个请求是在播放完唤醒词播报后发出的请求，此时系统已经进入到工作模式（非低功耗），
           这样时切换模型是合理的 */
        /*change asr normal word*/
        change_asr_normal_word();
    }
    else if(MSG_CMD_INFO_STATUS_EXIT_WAKEUP == cmd_info_msg->cmd_info_status)
    {
        /**
         * 由软定时器发起的请求，这里需要判断ignore_exit_wakeup变量，因为存在极端情况识别结果消息和该消息同时进入队列
         * 如果识别消息已经处理，则此时系统状态已经更新，这里便不可继续退出唤醒的动作，直接丢弃消息即可，否则可能覆盖了
         * 唤醒状态而未完全执行完毕所有的唤醒动作，产生错误。而如果此消息先处理，则会被识别消息刷新状态，不会产生问题
         */
        if(ignore_exit_wakeup == 0)
        {
            exit_wakeup_deal(1);
        }
        #if USE_CWSL
        else
        {
            ciss_set(CI_SS_START_SLEEP_PROCESS, 0);
        }
        #endif
    }
    else if(MSG_CMD_INFO_STATUS_ENABLE_EXIT_WAKEUP == cmd_info_msg->cmd_info_status)
    {
        /**
         * 由处理识别消息的位置发送的此消息，收到此消息后便可以继续接受正常的退出唤醒的消息了，因为系统状态更新的全流程
         * 执行完毕了此时再接受退出唤醒消息刷新系统状态之后完整进行退出唤醒流程是允许的。
         */
        if(ignore_exit_wakeup > 0)
        {
            ignore_exit_wakeup--;
        }
        CI_ASSERT(ignore_exit_wakeup>=0,"ignore_exit_wakeup err\n");
    }
    else if (MSG_CMD_INFO_STATUS_ENABLE_PROCESS_ASR == cmd_info_msg->cmd_info_status)
    {
        if (ignore_asr_msg > 0)
        {
            ignore_asr_msg--;
        }
    }
}

static void *weakup_asr_hander = NULL;
volatile bool asr_reseult_wakup_falg = false;
extern volatile int cias_wakeup_cmd_flag;

#if USE_CWSL
extern uint32_t deal_cwsl_msg_by_cmd_id(sys_msg_asr_data_t *asr_msg, cmd_handle_t *cmd_handle, uint16_t cmd_id);
static uint32_t sys_deal_cwsl_msg(sys_msg_asr_data_t *asr_msg, cmd_handle_t *cmd_handle, uint16_t cmd_id)
{
    uint32_t ret = 0;
    #if USE_CWSL
    ret = cwsl_app_process_asr_msg(asr_msg, cmd_handle, cmd_id);
    #endif
    return ret;
}

void sys_ignore_exit_msg_in_queue()
{
    if (SYS_STATE_WAKEUP == get_wakeup_state())
    {
        sys_msg_t send_msg;
        ignore_exit_wakeup++;
        send_msg.msg_type = SYS_MSG_TYPE_CMD_INFO;
        send_msg.msg_data.cmd_info_data.cmd_info_status = MSG_CMD_INFO_STATUS_ENABLE_EXIT_WAKEUP;
        send_msg_to_sys_task(&send_msg, NULL);
    }
}
/**
 * @brief 处理asr发送的消息
 *
 * @param asr_msg
 */
void sys_deal_asr_msg(sys_msg_asr_data_t *asr_msg)
{
    cmd_handle_t cmd_handle;
    int ret = 0;
    if((MSG_ASR_STATUS_GOOD_RESULT == asr_msg->asr_status||(MSG_CWSL_STATUS_GOOD_RESULT == asr_msg->asr_status)) && (ignore_asr_msg == 0))
    {
        int send_cancel_ignore_msg = 0;
		cmd_handle = (cmd_handle_t)asr_msg->asr_cmd_handle;
        if (SYS_STATE_WAKEUP == get_wakeup_state())
        {
            /* 这里已经唤醒了，所以目前已经在队列里的退出唤醒消息无效了，开启这个忽略退出唤醒消息的标志 */
            ignore_exit_wakeup++;
            send_cancel_ignore_msg = 1;
            uint16_t cmd_id = cmd_info_get_command_id(cmd_handle);
            ret = sys_deal_cwsl_msg(asr_msg, &cmd_handle, cmd_id);
        }

        ciss_set(CI_SS_CMD_SCORE,asr_msg->asr_score);
        if(cmd_info_is_wakeup_word(cmd_handle)) /*wakeup word*/
        {
            #if VOICE_UPLOAD_BY_UART
                mprintf("----voice_upload_enable---\n");
                voice_upload_enable();
                weakup_asr_hander = cmd_handle;
            #endif
            #if (VOICE_PLAY_BY_UART)
                cias_wakeup_cmd_flag = 1;
            #endif 
            /*updata wakeup state*/
            ciss_set(CI_SS_CMD_STATE,CI_SS_CMD_IS_WAKEUP);
            ciss_set(CI_SS_CMD_STATE_FOR_SSP,CI_SS_CMD_IS_WAKEUP);
            if (cwsl_get_status() != CWSL_STA_REG_TEMPLATE)
            {
                #if USE_IR_ENABEL
                uint32_t semantic_id = cmd_info_get_semantic_id(cmd_handle);
                if (MAIN_ASR_ONESHORT_OPEN_AIR == semantic_id)
                {
                    air_ctl(1,cmd_handle);
                }
                else if (MAIN_ASR_ONESHORT_CLOSE_AIR == semantic_id)
                {
                    air_ctl(0,cmd_handle);
                }
                user_led_light_on();
                user_led_light_flash();
                #endif
                enter_wakeup_deal(EXIT_WAKEUP_TIME,cmd_handle);    /*updata wakeup state*/
            }
        }
        else if (SYS_STATE_WAKEUP == get_wakeup_state())
        {
            #if VOICE_UPLOAD_BY_UART
            mprintf("----voice_upload_disable---\n");
            if(cmd_handle == weakup_asr_hander)
            {	
                asr_reseult_wakup_falg = true;
            }
            else
            {
                asr_reseult_wakup_falg = false;
                cias_send_cmd(LOCAL_ASR_RESULT_NOTIFY, DEF_FILL);
                voice_upload_disable();
                mprintf("====LOCAL_ASR_RESULT_NOTIFY===\n");
            }
            #endif
            ciss_set(CI_SS_CMD_STATE,CI_SS_CMD_IS_NORMAL);
            ciss_set(CI_SS_CMD_STATE_FOR_SSP,CI_SS_CMD_IS_NORMAL);
            set_state_enter_wakeup(EXIT_WAKEUP_TIME);

            uint32_t semantic_id = cmd_info_get_semantic_id(cmd_handle);
            uint16_t cmd_id = cmd_info_get_command_id(cmd_handle);
            ci_loginfo(LOG_USER,"asr cmd_id:%d,semantic_id:%08x\n", cmd_id, semantic_id);
            
            if (cwsl_get_status() != CWSL_STA_REG_TEMPLATE)
            {
                #ifdef IR_DEVICE_ALL
                userapp_deal_asr_msg_ex(asr_msg);      //处理语音控制红外，以及小夜灯等等接口
                #else
                //先按用户ID处理，如果用户没有处理，再按语义ID处理
                if (0 == deal_asr_msg_by_cmd_id(asr_msg, cmd_handle, cmd_id))
                {
                    if (0 == deal_asr_msg_by_semantic_id(asr_msg, cmd_handle, semantic_id))
                    {                    
                        #if PLAY_OTHER_CMD_EN
                        if(ret != 2)
                        {
                            #if (COMMAND_INFO_VER == 3)
                            if (cmd_info_is_auto_play(cmd_handle))
                            {
                                #if MULT_INTENT > 1
                                prompt_play_by_cmd_handle(cmd_handle, -1, default_play_done_callback,false);
                                #else
                                prompt_play_by_cmd_handle(cmd_handle, -1, default_play_done_callback,true);
                                #endif
                            }
                            #else
                            #if MULT_INTENT > 1
                            prompt_play_by_cmd_handle(cmd_handle, -1, default_play_done_callback,false);
                            #else
                            prompt_play_by_cmd_handle(cmd_handle, -1, default_play_done_callback,true);
                            #endif
                            #endif
                        }
                        #endif
                    }
                }
            #endif
            }
        }

        if (send_cancel_ignore_msg)
        {
            /* 发送一个取消忽略退出唤醒的消息到队列尾部，处理该消息后就允许退出唤醒了 */
            sys_msg_t send_msg;
            send_msg.msg_type = SYS_MSG_TYPE_CMD_INFO;
            send_msg.msg_data.cmd_info_data.cmd_info_status = MSG_CMD_INFO_STATUS_ENABLE_EXIT_WAKEUP;
            send_msg_to_sys_task(&send_msg, NULL);
        }

        if (cwsl_get_status() != CWSL_STA_REG_TEMPLATE)
        {
            sys_asr_result_hook(cmd_handle, asr_msg->asr_score);
        }
    }
    else if((MSG_ASR_STATUS_NO_RESULT == asr_msg->asr_status)||
        (MSG_ASR_STATUS_VAD_END == asr_msg->asr_status))
    {
        sys_manage_data.asr_state = SYS_STATE_ASR_IDLE;/*idle*/
    }
    else if(MSG_ASR_STATUS_VAD_START == asr_msg->asr_status)
    {
        sys_manage_data.asr_state = SYS_STATE_ASR_BUSY;/*busy*/
    }
}
#else
/**
 * @brief 处理asr发送的消息
 *
 * @param asr_msg
 */
void sys_deal_asr_msg(sys_msg_asr_data_t *asr_msg)
{
    cmd_handle_t cmd_handle;

    if(MSG_ASR_STATUS_GOOD_RESULT == asr_msg->asr_status && (ignore_asr_msg == 0))
    {
        int send_cancel_ignore_msg = 0;
		cmd_handle = (cmd_handle_t)asr_msg->asr_cmd_handle;
        if (SYS_STATE_WAKEUP == get_wakeup_state())
        {
            /* 这里已经唤醒了，所以目前已经在队列里的退出唤醒消息无效了，开启这个忽略退出唤醒消息的标志 */
            ignore_exit_wakeup++;
            send_cancel_ignore_msg = 1;
        }

        ciss_set(CI_SS_CMD_SCORE,asr_msg->asr_score);
        if(cmd_info_is_wakeup_word(cmd_handle)) /*wakeup word*/
        {
            #if VOICE_UPLOAD_BY_UART
                mprintf("----voice_upload_enable---\n");
                voice_upload_enable();
                weakup_asr_hander = cmd_handle;
            #endif
            #if (VOICE_PLAY_BY_UART)
                cias_wakeup_cmd_flag = 1;
            #endif  
            #if USE_IR_ENABEL
            uint32_t semantic_id = cmd_info_get_semantic_id(cmd_handle);
            if (MAIN_ASR_ONESHORT_OPEN_AIR == semantic_id)
            {
                air_ctl(1,cmd_handle);
            }
            else if (MAIN_ASR_ONESHORT_CLOSE_AIR == semantic_id)
            {
                air_ctl(0,cmd_handle);
            }
            user_led_light_on();
            user_led_light_flash();
            #endif
            /*updata wakeup state*/
            ciss_set(CI_SS_CMD_STATE,CI_SS_CMD_IS_WAKEUP);
            ciss_set(CI_SS_CMD_STATE_FOR_SSP,CI_SS_CMD_IS_WAKEUP);
            enter_wakeup_deal(EXIT_WAKEUP_TIME,cmd_handle);    /*updata wakeup state*/

        }
        else if (SYS_STATE_WAKEUP == get_wakeup_state())
        {
        #if VOICE_UPLOAD_BY_UART
            mprintf("----voice_upload_disable---\n");
            if(cmd_handle == weakup_asr_hander)
            {	
                asr_reseult_wakup_falg = true;
            }
            else
            {
                asr_reseult_wakup_falg = false;
                cias_send_cmd(LOCAL_ASR_RESULT_NOTIFY, DEF_FILL);
                voice_upload_disable();
                mprintf("====LOCAL_ASR_RESULT_NOTIFY===\n");
            }
        #endif
            ciss_set(CI_SS_CMD_STATE,CI_SS_CMD_IS_NORMAL);
            ciss_set(CI_SS_CMD_STATE_FOR_SSP,CI_SS_CMD_IS_NORMAL);
            set_state_enter_wakeup(EXIT_WAKEUP_TIME);
            #if USE_WMAN_VPR   //男女声纹判断
            vpr_run_one_recognition();
            #endif
            uint32_t semantic_id = cmd_info_get_semantic_id(cmd_handle);
            uint16_t cmd_id = cmd_info_get_command_id(cmd_handle);
            ci_loginfo(LOG_USER,"asr cmd_id:%d,semantic_id:%08x\n", cmd_id, semantic_id);
            #ifdef IR_DEVICE_ALL
            userapp_deal_asr_msg_ex(asr_msg);      //处理语音控制红外，以及小夜灯等等接口
            #else
            //先按用户ID处理，如果用户没有处理，再按语义ID处理
            if (0 == deal_asr_msg_by_cmd_id(asr_msg, cmd_handle, cmd_id))
            {
                if (0 == deal_asr_msg_by_semantic_id(asr_msg, cmd_handle, semantic_id))
                {                    
                    #if PLAY_OTHER_CMD_EN && !WMAN_PLAY_EN
                    #if (COMMAND_INFO_VER == 3)
                    if (cmd_info_is_auto_play(cmd_handle))
                    {
                        #if MULT_INTENT > 1
                        prompt_play_by_cmd_handle(cmd_handle, -1, default_play_done_callback,false);
                        #else
                        prompt_play_by_cmd_handle(cmd_handle, -1, default_play_done_callback,true);
                        #endif
                    }
                    #else
                    #if MULT_INTENT > 1
                    prompt_play_by_cmd_handle(cmd_handle, -1, default_play_done_callback,false);
                    #else
                    prompt_play_by_cmd_handle(cmd_handle, -1, default_play_done_callback,true);
                    #endif
                    #endif
                    #endif
                }
            }
            #endif
        }

        if (send_cancel_ignore_msg)
        {
            /* 发送一个取消忽略退出唤醒的消息到队列尾部，处理该消息后就允许退出唤醒了 */
            sys_msg_t send_msg;
            send_msg.msg_type = SYS_MSG_TYPE_CMD_INFO;
            send_msg.msg_data.cmd_info_data.cmd_info_status = MSG_CMD_INFO_STATUS_ENABLE_EXIT_WAKEUP;
            send_msg_to_sys_task(&send_msg, NULL);
        }
        sys_asr_result_hook(cmd_handle, asr_msg->asr_score);
    }
    else if((MSG_ASR_STATUS_NO_RESULT == asr_msg->asr_status)||
        (MSG_ASR_STATUS_VAD_END == asr_msg->asr_status))
    {
        sys_manage_data.asr_state = SYS_STATE_ASR_IDLE;/*idle*/
    }
    else if(MSG_ASR_STATUS_VAD_START == asr_msg->asr_status)
    {
        sys_manage_data.asr_state = SYS_STATE_ASR_BUSY;/*busy*/
    }
}
#endif

/**
 * @brief system message deal function and user main UI flow. system message include ASR, player, KEY, COM
 *
 * @param p_arg
 */


void UserTaskManageProcess(void *p_arg)
{
    sys_msg_t rev_msg;
    BaseType_t err = pdPASS;
    /* 上电初始化状态 */
    #if (USE_BEAMFORMING_MODULE && USE_AEC_MODULE)
    sys_manage_data.wakeup_state = SYS_STATE_WAKEUP;//SYS_STATE_UNWAKEUP;
    #else
    sys_manage_data.wakeup_state = SYS_STATE_UNWAKEUP;
    #endif
    vTaskSuspendAll();

    #if (USE_BEAMFORMING_MODULE && USE_AEC_MODULE)
    ciss_set(CI_SS_WAKING_UP_STATE,CI_SS_WAKEUPED);
    ciss_set(CI_SS_WAKING_UP_STATE_FOR_SSP,CI_SS_WAKEUPED);
    #else
    ciss_set(CI_SS_WAKING_UP_STATE,CI_SS_NO_WAKEUP);
    ciss_set(CI_SS_WAKING_UP_STATE_FOR_SSP,CI_SS_NO_WAKEUP);
    #endif    
    xTaskResumeAll();

   // nlp_timer_init();
    /* 退出唤醒timer和led pwm控制timer 初始化 */
    #if USE_IR_ENABEL
    exit_wakeup_timer = xTimerCreate("exit_wakeup", pdMS_TO_TICKS(exit_wakeup_timeout),
        pdFALSE, (void *)0, exit_wakeup_timer_callback);
    #else
    exit_wakeup_timer = xTimerCreate("exit_wakeup", pdMS_TO_TICKS(EXIT_WAKEUP_TIME),
        pdFALSE, (void *)0, exit_wakeup_timer_callback);
    #endif

    if(NULL == exit_wakeup_timer)
    {
        ci_logerr(LOG_USER,"user task create timer error\n");
    }
    #if USE_LOWPOWER_OSC_FREQUENCY
    /* osc时钟低功耗模式使用的timer，用于vad start但未唤醒的超时timer */
    exit_down_freq_mode_timer = xTimerCreate("exit_down_freq_mode", pdMS_TO_TICKS(15000),
        pdFALSE, (void *)0, exit_down_freq_mode_cb);
    #endif
    while(1)
    {
        /* 阻塞接收系统消息 */
        err = xQueueReceive(sys_msg_queue, &rev_msg, portMAX_DELAY);

        if(pdPASS == err)
        {
            /* 根据消息来源来处理对应消息，用户可以自己创建属于自己的系统消息类型 */
            switch (rev_msg.msg_type)
            {
#if USE_SED
                case SYS_MSG_TYPE_SED:
                {
                    sed_rslt_out();
                    break;
                }
#endif
                /* 来自ASR的消息，主要通知识别结果，asr状态 */
                case SYS_MSG_TYPE_ASR:
                {
                    sys_msg_asr_data_t *asr_rev_data;
                    asr_rev_data = &(rev_msg.msg_data.asr_data);
                    sys_deal_asr_msg(asr_rev_data);
                    break;
                }


                /* 系统控制消息，主要通知处理识别模型切换以及低功耗模式切换的请求 */
                case SYS_MSG_TYPE_CMD_INFO:
                {
                    sys_msg_cmd_info_data_t *cmd_info_rev_data;
                    cmd_info_rev_data = &(rev_msg.msg_data.cmd_info_data);
                    sys_deal_cmd_info_msg(cmd_info_rev_data);
                    break;
                }
                /* 音频采集任务消息，目前处理音频采集开启完成，在这里播放欢迎词 */
                case SYS_MSG_TYPE_AUDIO_IN_STARTED:
                {
                    uint8_t volume;
                    uint16_t real_len;

                    /* 从nvdata里读取播放音量 */
                    if(CINV_OPER_SUCCESS != cinv_item_read(NVDATA_ID_VOLUME, sizeof(volume), &volume, &real_len))
                    {
                        /* nvdata内无播放音量则配置为初始默认音量并写入nv */
                        volume = VOLUME_DEFAULT;
                        cinv_item_init(NVDATA_ID_VOLUME, sizeof(volume), &volume);
                    }
                    /* 音量设置 */
                    vol_set(volume);

                    #if (EXCEPTION_RST_SKIP_BOOT_PROMPT)
                    if (RETURN_OK != scu_get_system_reset_state())
                    {
                        /* 本次开机属于异常复位，不需要播放欢迎词直接进入切换非唤醒模式流程 */
                        sys_msg_t send_msg;
                        send_msg.msg_type = SYS_MSG_TYPE_CMD_INFO;
                        send_msg.msg_data.cmd_info_data.cmd_info_status = MSG_CMD_INFO_STATUS_POST_CHANGE_ASR_WAKEUP_WORD;
                        send_msg_to_sys_task(&send_msg, NULL);
                    }
                    else
                    #endif
                    {
                        #if PLAY_WELCOME_EN
                        vTaskDelay(pdMS_TO_TICKS(300));     // 等待功放开启
                        play_exit_wakeup_done_cb(INVALID_HANDLE);
                        /* mute语音输入并播放欢迎词，播放完毕后调用play_exit_wakeup_done_cb回调以开启语音输入并进入切换非唤醒模式流程 */
                        prompt_play_by_cmd_string("<welcome>", -1, NULL,true);
                        #else
                        play_exit_wakeup_done_cb(INVALID_HANDLE);
                        #endif
                        #if USE_VPR || USE_WMAN_VPR   //算法带加密功能，播放加密状态
                        while (REMOTE_CALL(get_aes_state()) == -1);
                        {
                            vTaskDelay(pdMS_TO_TICKS(5));
                        }
                        if(REMOTE_CALL(get_aes_state()) == 2)  //未烧录量产密码
                        {
                            prompt_play_by_cmd_string("<alg_aes_check>", 1, NULL, false);
                        }
                        #endif
                    }
					/* 通过串口协议发送模块上电通知 */
                    #if (EXCEPTION_RST_SKIP_BOOT_PROMPT)
                    if (RETURN_OK == scu_get_system_reset_state())
                    #endif
                    #if !UART_BAUDRATE_CALIBRATE
                    {
					    sys_power_on_hook();
                    }
                    #endif
                    break;
                }
                default:
                    deal_userdef_msg(&rev_msg);
                    break;
            }

            switch(sys_manage_data.user_msg_state)
            {
                case SYS_STATE_WAKUP_TIMEOUT:
                {
                    if(SYS_STATE_ASR_IDLE == get_asr_state())
                    {
                        exit_wakeup_deal(0);
                        sys_manage_data.user_msg_state = USERSTATE_WAIT_MSG;
                    }
                    break;
                }
                default:
                    break;
            }
        }
    }
}

#if USE_PWK
void ci_pwk_get_cb(int db_val)
{
	mprintf("--------pwk db val: %ddb\n", db_val);
}
#endif

