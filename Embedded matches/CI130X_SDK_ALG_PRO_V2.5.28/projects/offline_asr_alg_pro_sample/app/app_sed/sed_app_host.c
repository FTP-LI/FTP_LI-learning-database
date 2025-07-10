#include "ci_log.h"
#include <stdint.h>
#include "sed_manage.h"
#include "sed_app_host.h"
#include "prompt_player.h"
#include "audio_play_process.h"
#include "ci130x_gpio.h"
#include "FreeRTOS.h"
#include "system_msg_deal.h"
#include "ci130x_dpmu.h"
#include "ci_nvdata_manage.h"
#include "remote_api_for_host.h"
#if USE_SED

#if USE_SED_CRY
const sed_param_config_t sed_config_para = 
{
	.threshold = THRESHOLD_CRY,
	.times = TIMES_CRY
};
#elif USE_SED_SNORE
const sed_param_config_t sed_config_para = 
{
	.threshold = THRESHOLD_SNORE,
	.times = TIMES_SNORE
};
#endif


static int out_flag = 0; 
static int times = 0;
static bool sed_relt_flag = false;



bool sed_continuous_detect(float out_array)
{  
    float thr = sed_config_para.threshold;
#if USE_SED_CRY
    if(sed_config_para.times <= TIMES_CRY)
#elif USE_SED_SNORE
    if(sed_config_para.times <= TIMES_SNORE)
#endif
    {
        times ++ ;
        if (out_array >= thr)
        {
            out_flag++;
        }
        else
        {
            out_flag--;
        }
        if(times == sed_config_para.times )
        {
            if (out_flag > 0)
            {
                sed_relt_flag = true;
                mprintf("***** Detected *****\n");
                sys_msg_t send_msg;
                send_msg.msg_type = SYS_MSG_TYPE_SED;
                send_msg_to_sys_task(&send_msg, NULL);
            }
            else
            {
                sed_relt_flag = false;
                mprintf("***** None *****\n");
            }

            times = 0;
            out_flag = 0;
        }
    }

    return sed_relt_flag;
}


const uint8_t out_uart[] = {
    0xA5,
    0xFA,
    0x00,
    0x81,
    0x01,
    0x00};


void sed_rslt_cb(void* data)
{
    float* data_sed = (float*)data;
    mprintf("out[0] = %d\n",(int)(data_sed[0] * 1000.0f));
    sed_relt_flag = sed_continuous_detect(data_sed[0]);
}




/**
 * @brief IO 输出初始化
 */
void io_out_init(void)
{
#if 0
    scu_set_device_gate((unsigned int)SED_DETECTED_GPIO_BASE, ENABLE);        // 开启PA时钟
    dpmu_set_io_reuse(SED_DETECTED_GPIO, FIRST_FUNCTION);               // 设置引脚功能复用为GPIO
    dpmu_set_io_direction(SED_DETECTED_GPIO, DPMU_IO_DIRECTION_OUTPUT); // 设置引脚功能为输出模式
    dpmu_set_io_pull(SED_DETECTED_GPIO, DPMU_IO_PULL_DISABLE);          // 设置关闭上下拉
    gpio_set_output_mode(SED_DETECTED_GPIO_BASE, SED_DETECTED_GPIO_PIN);                      // GPIO的pin脚配置成输出模式
    gpio_set_output_level_single(SED_DETECTED_GPIO_BASE, SED_DETECTED_GPIO_PIN, SED_DETECTED_DEFAULT_VAL);           // 输出高电平
#endif
}


/**
 * @brief 设置音量初始化
 */
void sed_set_vol_init(void)
{
    uint8_t volume;
    uint16_t real_len;

    /* 从nvdata里读取播放音量 */
    if (CINV_OPER_SUCCESS != cinv_item_read(NVDATA_ID_VOLUME, sizeof(volume), &volume, &real_len))
    {
        /* nvdata内无播放音量则配置为初始默认音量并写入nv */
        volume = VOLUME_DEFAULT;
        cinv_item_init(NVDATA_ID_VOLUME, sizeof(volume), &volume);
    }
     /* 音量设置 */
    vol_set(volume);
}

/**
 * @brief 播报欢迎词
 */
void sed_play_welcome_prompt(void)
{
	#if PLAY_WELCOME_EN
    vTaskDelay(pdMS_TO_TICKS(300)); // 等待功放开启

    /* mute语音输入并播放欢迎词，播放完毕后调用play_exit_wakeup_done_cb回调以开启语音输入并进入切换非唤醒模式流程 */
    prompt_play_by_cmd_string("<welcome>", 1, NULL,true); //播放提示音
	#endif
}

#if CLOUD_UART_PROTOCOL_EN
#include "chipintelli_cloud_protocol.h"
#if USE_SED_CRY 
extern SedCryCloudCmdProtocolTypedef gSedCryCloudCmdProtocol;
#endif
#if USE_SED_SNORE 
extern SedSnoreCloudCmdProtocolTypedef gSedSnoreCloudCmdProtocol;
#endif
void sed_rslt_out(void)
{
    uint32_t state = get_audio_play_state();
    if (AUDIO_PLAY_STATE_IDLE == state)
    {
        sed_relt_flag = 0;

        #if USE_SED_CRY
            #if CLOUD_CFG_PLAY_EN
            if(gSedCryCloudCmdProtocol.cloudCmdProtocol.play_type == 0)  //主动播报
                prompt_play_by_cmd_id(gSedCryCloudCmdProtocol.cloudCmdProtocol.cmd_id, -1, default_play_done_callback, true);
            #endif
            #if CLOUD_CFG_UART_SEND_EN
                cloud_uart_send_buf(gSedCryCloudCmdProtocol.cloudCmdProtocol.protocol_buf, gSedCryCloudCmdProtocol.cloudCmdProtocol.protocol_len);
            #endif
        #elif USE_SED_SNORE
            #if CLOUD_CFG_PLAY_EN
            if(gSedSnoreCloudCmdProtocol.cloudCmdProtocol.play_type == 0)  //主动播报
                prompt_play_by_cmd_id(gSedSnoreCloudCmdProtocol.cloudCmdProtocol.cmd_id, -1, default_play_done_callback, true);
            #endif
            #if CLOUD_CFG_UART_SEND_EN
                cloud_uart_send_buf(gSedSnoreCloudCmdProtocol.cloudCmdProtocol.protocol_buf, gSedSnoreCloudCmdProtocol.cloudCmdProtocol.protocol_len);
            #endif
        #endif
    }
}
#else
/**
 * @brief 输出结果
 */
void sed_rslt_out(void)
{

    uint32_t state = get_audio_play_state();
    if (AUDIO_PLAY_STATE_IDLE == state)
    {
        #if USE_SED_CRY
            prompt_play_by_cmd_id(SED_DETECTED_CRY, -1, NULL, true);
        #elif USE_SED_SNORE
            prompt_play_by_cmd_id(SED_DETECTED_SNORE, -1, NULL, true);
        #endif
    }
    sed_relt_flag = 0;
}
#endif
#endif