/**
 * @file main.c
 * @brief 示例程序
 * @version 1.0.0
 * @date 2021-03-19
 *
 * @copyright Copyright (c) 2019  Chipintelli Technology Co., Ltd.
 *
 */
#include <stdio.h> 
#include <malloc.h>
#include "FreeRTOS.h" 
#include "task.h"
#include "sdk_default_config.h"
#include "ci130x_core_eclic.h"
#include "ci130x_spiflash.h"
#include "ci130x_gpio.h"
#include "ci130x_dma.h"
#include "audio_play_api.h"
#include "audio_play_decoder.h"
#include "ci_flash_data_info.h"
// #include "ci130x_audio_capture.h"
#include "board.h"
#include "ci130x_uart.h"
#include "flash_manage_outside_port.h"
#include "system_msg_deal.h"
#include "nlp_msg_deal.h"
#include "ci130x_dpmu.h"
#include "ci130x_mailbox.h"
#include "ci130x_nuclear_com.h"
#include "flash_control_inner_port.h"
#include "romlib_runtime.h"
#include "audio_in_manage_inner.h"
#include "ci_log.h"
#include "status_share.h"
#include "platform_config.h"
#include "asr_api.h"
#include "alg_preprocess.h"
#include "ci130x_iwdg.h"
#include "voice_print_recognition.h"
#include "cwsl_manage.h"
#include "codec_manager.h"
#include "cias_audio_data_handle.h"
#include "ota_partition_verify.h"
#include "doa_app_handle.h"
#include "codec_manage_outside_port.h"
#include "code_switch.h"
#include "ci_nlp_user.h"
#include "ci_nlp.h"
/**
 * @brief 硬件初始化
 *          这个函数主要用于系统上电后初始化硬件寄存器到初始值，配置中断向量表初始化芯片io配置时钟
 *          配置完成后，系统时钟配置完毕，相关获取clk的函数可以正常调用
 */
static void hardware_default_init(void)
{
    /* 配置外设复位，硬件外设初始化 */
	extern void SystemInit(void);
    SystemInit();

	/* 设置中断优先级分组 */
	eclic_priority_group_set(ECLIC_PRIGROUP_LEVEL3_PRIO0);
    
	/* 开启全局中断 */
	eclic_global_interrupt_enable();

	enable_mcycle_minstret();

	init_platform();

    /* 初始化maskrom lib */
    maskrom_lib_init();

    #if !(USE_INNER_LDO3)
    dpmu_ldo3_en(false);
    dpmu_config_update_en(DPMU_UPDATE_EN_NUM_LDO3);
    #endif

    //DMA通道中断开启
    scu_set_dma_mode(DMAINT_SEL_CHANNEL1);
    scu_set_device_reset(HAL_GDMA_BASE);
    scu_set_device_reset_release(HAL_GDMA_BASE);
}


/**
 * @brief 用于平台初始化相关代码
 *
 * @note 在这里初始化硬件需要注意：
 *          由于部分驱动代码中使用os相关接口，在os运行前调用这些接口会导致中断被屏蔽
 *          其中涉及的驱动包括：QSPIFLASH、DMA、I2C、SPI
 *          所以这些外设的初始化需要放置在vTaskVariablesInit进行。
 *          如一定需要（非常不建议）在os运行前初始化这些驱动，请仔细确认保证：
 *              1.CONFIG_DIRVER_BUF_USED_FREEHEAP_EN  宏配置为0
 *              2.DRIVER_OS_API                     宏配置为0
 */
static int platform_init(void)
{   
    #if CONFIG_CI_LOG_UART
    ci_log_init();      //初始化日志模块
    #if COMMAND_LINE_CONSOLE_EN         
    vUARTCommandConsoleStart( 256, 4);
    #endif
    #endif
    #if (CONFIG_CI_LOG_UART == UART_PROTOCOL_NUMBER && MSG_COM_USE_UART_EN)
	CI_ASSERT(0,"Log uart and protocol uart confict!\n");
    #endif
    
    #if CONFIG_SYSTEMVIEW_EN   
    /* 初始化SysView RTT，仅用于调试 */
	SEGGER_SYSVIEW_Conf();
	/* 使用串口方式输出sysview信息 */
	vSYSVIEWUARTInit();
	ci_logdebug(CI_LOG_DEBUG, "Segger Sysview Control Block Detection Address is 0x%x\n",&_SEGGER_RTT);
#endif
#if 1   //开启看门狗
    iwdg_init_t init;
    init.irq = iwdg_irqen_enable;
    init.res = iwdg_resen_enable;
    init.count = ((get_src_clk()/0x10)*3);/* IWDG时钟从src_clk经过16分频得到, 当前配置为2秒*/
    scu_set_device_gate(IWDG, ENABLE);
    dpmu_iwdg_reset_system_config();
    iwdg_init(IWDG,init);
    iwdg_open(IWDG);
#endif
	return 0;
}


/**
 * @brief sdk上电信息打印
 *
 */
static void welcome(void)
{
    ci_loginfo(LOG_USER,"\r\n");
    ci_loginfo(LOG_USER,"\r\n");
    ci_loginfo(LOG_USER,"ci130x_sdk_%s_%d.%d.%d Built-in\r\n",
               SDK_TYPE,
               SDK_VERSION,SDK_SUBVERSION,SDK_REVISION);
    ci_loginfo(LOG_USER,"\033[1;32mWelcome to CI130x_SDK.\033[0;39m\r\n");
    extern char heap_start;
    extern char heap_end;
    ci_loginfo(LOG_USER,"Heap size:%dKB\n", (((uint32_t)&heap_end) - ((uint32_t)&heap_start))/1024);
    ci_loginfo(LOG_USER,"Freq factor %d\n", (int)(get_freq_factor()*1000));
    ci_loginfo(LOG_USER,"Freq %d\n", (int)(get_ipcore_clk()));

    // 实际主频检查
    if (abs(((int)get_ipcore_clk()) - ((int)MAIN_FREQUENCY)) > 10000000)
    {
        mprintf("PLL config err!\n");
        while(1);
    }
}
//算法模型初始化
static int alg_model_init(void)
{
    #if USE_VPR
    vpr_init(vpr_callback);
    #endif
    #if USE_WMAN_VPR
    vpr_init(NULL); 
    #endif
    #if USE_DENOISE_NN
    get_ci_ai_denoise_model_addr();
    #endif
    #if USE_AI_DOA
    get_ci_ai_doa_model_addr();
    #if !USE_AEC_MODULE
    REMOTE_CALL(set_doa_out_type(AI_DOA_OUT_TYPE));
    #endif
    #endif
    #if USE_SED  
    REMOTE_CALL(sed_nn_cmpt_cfg(200,0));     //配置哭声检测一次的帧数，10ms一帧
    extern void get_ci_sed_model_addr(void);
    get_ci_sed_model_addr();
    #endif
    return 0;
}
#if CLOUD_UART_PROTOCOL_EN
#include "chipintelli_cloud_protocol.h"
//云端协串口议初始化
static int alg_cloud_protocol_init(void)
{
    #if USE_VPR
        vpr_cloud_cmd_init_call();
    #elif USE_WMAN_VPR
        vgr_cloud_cmd_init_call();
    #elif USE_SED_CRY
        cry_cloud_cmd_init_call();
    #elif USE_SED_SNORE     
        snore_cloud_cmd_init_call();
    #elif USE_AI_DOA     
        doa_cloud_cmd_init_call();
    #endif
    return 0;
}
#endif

static void task_init(void *p_arg)
{
	#if USE_BLE
    ble_xtal_init();
    welcome();                   //231x蓝牙版本信息打印延后
    #endif
    #if USE_CWSL&&USE_AI_DOA&&USE_DEREVERB_MODULE&&USE_AEC_MODULE
    extern char SDK_ALG_PRO_SRAM_HOST_CWSL_DOA_DEREVERB_AEC_END_ADDR;
    dsu_init((uint32_t)&SDK_ALG_PRO_SRAM_HOST_CWSL_DOA_DEREVERB_AEC_END_ADDR);
    #elif USE_AI_DOA&&USE_DEREVERB_MODULE&&USE_AEC_MODULE
    extern char SDK_ALG_PRO_SRAM_HOST_DOA_DEREVERB_AEC_END_ADDR;
    dsu_init((uint32_t)&SDK_ALG_PRO_SRAM_HOST_DOA_DEREVERB_AEC_END_ADDR);
    #elif USE_CWSL&&USE_AI_DOA&&USE_AEC_MODULE
    extern char SDK_ALG_PRO_SRAM_HOST_CWSL_DOA_AEC_END_ADDR;
    dsu_init((uint32_t)&SDK_ALG_PRO_SRAM_HOST_CWSL_DOA_AEC_END_ADDR);
    #elif USE_CWSL&&USE_DUAL_MIC_ANY&&USE_AEC_MODULE
    extern char SDK_ALG_PRO_SRAM_HOST_CWSL_ANY_MIC_AEC_END_ADDR;
    dsu_init((uint32_t)&SDK_ALG_PRO_SRAM_HOST_CWSL_ANY_MIC_AEC_END_ADDR);
    #elif USE_BEAMFORMING_MODULE&&USE_AEC_MODULE
    extern char SDK_ALG_PRO_SRAM_HOST_BF_AEC_END_ADDR;
    dsu_init((uint32_t)&SDK_ALG_PRO_SRAM_HOST_BF_AEC_END_ADDR);
    #elif USE_BEAMFORMING_MODULE && USE_DEREVERB_MODULE
    extern char SDK_ALG_PRO_SRAM_HOST_BF_DEREVERB_END_ADDR;
    dsu_init((uint32_t)&SDK_ALG_PRO_SRAM_HOST_BF_DEREVERB_END_ADDR);
    #elif USE_AI_DOA&&USE_AEC_MODULE
    extern char SDK_ALG_PRO_SRAM_HOST_DOA_AEC_END_ADDR;
    dsu_init((uint32_t)&SDK_ALG_PRO_SRAM_HOST_DOA_AEC_END_ADDR);
    #elif USE_AI_DOA&&USE_DEREVERB_MODULE
    extern char SDK_ALG_PRO_SRAM_HOST_DOA_DEREVERB_END_ADDR;
    dsu_init((uint32_t)&SDK_ALG_PRO_SRAM_HOST_DOA_DEREVERB_END_ADDR);
	#elif USE_DEREVERB_MODULE&&USE_AEC_MODULE
    extern char SDK_ALG_PRO_SRAM_HOST_DEREVERB_AEC_END_ADDR;
    dsu_init((uint32_t)&SDK_ALG_PRO_SRAM_HOST_DEREVERB_AEC_END_ADDR);
    #elif USE_CWSL&&USE_AI_DOA
    extern char SDK_ALG_PRO_SRAM_HOST_CWSL_DOA_END_ADDR;
    dsu_init((uint32_t)&SDK_ALG_PRO_SRAM_HOST_CWSL_DOA_END_ADDR);
    #elif USE_CWSL&&USE_AEC_MODULE
    extern char SDK_ALG_PRO_SRAM_HOST_CWSL_AEC_END_ADDR;
    dsu_init((uint32_t)&SDK_ALG_PRO_SRAM_HOST_CWSL_AEC_END_ADDR);
    #elif USE_DUAL_MIC_ANY&&USE_AEC_MODULE
    extern char SDK_ALG_PRO_SRAM_HOST_ANY_MIC_AEC_END_ADDR;
    dsu_init((uint32_t)&SDK_ALG_PRO_SRAM_HOST_ANY_MIC_AEC_END_ADDR);
    #elif USE_BEAMFORMING_MODULE
    extern char SDK_ALG_PRO_SRAM_HOST_BF_END_ADDR;
    dsu_init((uint32_t)&SDK_ALG_PRO_SRAM_HOST_BF_END_ADDR);
    #elif USE_CWSL
    extern char SDK_ALG_PRO_SRAM_HOST_CWSL_END_ADDR;
    dsu_init((uint32_t)&SDK_ALG_PRO_SRAM_HOST_CWSL_END_ADDR);
    #elif USE_DENOISE_NN 
    extern char SDK_ALG_PRO_SRAM_HOST_DENOISE_END_ADDR;
    dsu_init((uint32_t)&SDK_ALG_PRO_SRAM_HOST_DENOISE_END_ADDR);  
    #elif USE_AI_DOA
    extern char SDK_ALG_PRO_SRAM_HOST_DOA_END_ADDR;
    dsu_init((uint32_t)&SDK_ALG_PRO_SRAM_HOST_DOA_END_ADDR);  
    #elif USE_VPR
    extern char SDK_ALG_PRO_SRAM_HOST_VPR_END_ADDR;
    dsu_init((uint32_t)&SDK_ALG_PRO_SRAM_HOST_VPR_END_ADDR);  
    #elif USE_WMAN_VPR
    extern char SDK_ALG_PRO_SRAM_HOST_WMANVPR_END_ADDR;
    dsu_init((uint32_t)&SDK_ALG_PRO_SRAM_HOST_WMANVPR_END_ADDR);  
    #elif USE_SED
    extern char SDK_ALG_PRO_SRAM_HOST_SED_END_ADDR;
    dsu_init((uint32_t)&SDK_ALG_PRO_SRAM_HOST_SED_END_ADDR);
     #elif USE_TTS
    extern char SDK_ALG_PRO_SRAM_HOST_TTS_END_ADDR;
    dsu_init((uint32_t)&SDK_ALG_PRO_SRAM_HOST_TTS_END_ADDR);
    #elif USE_AEC_MODULE
    extern char SDK_ALG_PRO_SRAM_HOST_AEC_END_ADDR;
    dsu_init((uint32_t)&SDK_ALG_PRO_SRAM_HOST_AEC_END_ADDR);
    #elif USE_DEREVERB_MODULE
    extern char SDK_ALG_PRO_SRAM_HOST_DEREVERB_END_ADDR;
    dsu_init((uint32_t)&SDK_ALG_PRO_SRAM_HOST_DEREVERB_END_ADDR);
    #else
    extern char SDK_ALG_PRO_SRAM_HOST_NULL_END_ADDR;
    dsu_init((uint32_t)&SDK_ALG_PRO_SRAM_HOST_NULL_END_ADDR);
    #endif
    vTaskDelay(pdMS_TO_TICKS(5));       //必须延时5ms，否则识别慢
    #if CLOUD_UART_PROTOCOL_EN
    UARTPollingConfig((UART_TypeDef*)CLOUD_CFG_UART_PORT, CLOUD_CFG_UART_BAUND_RATE);
    #endif

    cm_init();

    /* 注册录音codec */
    audio_in_codec_registe();
    
    nuclear_com_init();

    /*各个通信组件的初始化*/
    decoder_port_inner_rpmsg_init();
    flash_control_inner_port_init();
    dnn_nuclear_com_outside_port_init();
    asr_top_nuclear_com_outside_port_init();
    vad_fe_nuclear_com_outside_port_init();
    flash_manage_nuclear_com_outside_port_init();
    codec_manage_inner_port_init();
    ciss_init();
    ciss_set(CI_SS_AUDIO_IN_BUFFER_NUM, AUDIO_IN_BUFFER_NUM);
    ciss_set(CI_SS_DECODER_MIN_ACTIVE,DECODER_MIN_ACTIVE);
    ciss_set(CI_SS_INTENT_NUM, MULT_INTENT);
    ciss_set(CI_SS_HOST_PARAM_SET_OK, 0x5A);  //参数设置完成
    #if DEEP_SEPARATE_ENABLE
    ciss_set(CI_SS_CHA_NUM, 2);
    #else 
    ciss_set(CI_SS_CHA_NUM, 1);
    #endif
    
    float beam = DECODER_BEAM;
    ciss_set(CI_SS_DECODER_BEAM,*(uint32_t*)&beam);

	mailboxboot_sync();

    #if CI_OTA_ENABLE    //如果使能ota, 检测ota是否升级完成,防止上次升级失败无法重启
    check_ota_finish();
    #endif
	#if USE_AEC_MODULE&&USE_AI_DOA
    ciss_set(CI_SS_AEC_WORK_STATE, 0);
    ciss_set(CI_SS_DOA_WORK_STATE, 1);
    ciss_set(CI_SS_DOA_INIT_STATE, 1);
    ciss_set(CI_SS_CUR_DOA_AEC_STATE, 1);
    #elif USE_AEC_MODULE
    ciss_set(CI_SS_DOA_WORK_STATE, 0);
    ciss_set(CI_SS_AEC_WORK_STATE, 1);
    ciss_set(CI_SS_AEC_INIT_STATE, 1);
    #elif USE_AI_DOA
    ciss_set(CI_SS_AEC_WORK_STATE, 0);
    ciss_set(CI_SS_DOA_WORK_STATE, 1);
    ciss_set(CI_SS_DOA_INIT_STATE, 1);
    #endif

    //注册语音前段信号处理模块
    #if USE_DUAL_MIC_ANY
    ciss_set(CI_SS_DUALMIC_IS_ANY,true);
    ciss_set(CI_SS_ANY_MIC_AEC_PCM_THR_VAL, 100);
    cm_set_codec_adc_gain(HOST_MIC_RECORD_CODEC_ID, CM_CHA_LEFT,  DISABLE);   //禁用alc，设置固定增益为20
    cm_set_codec_adc_gain(HOST_MIC_RECORD_CODEC_ID, CM_CHA_LEFT, 20);
    cm_set_codec_adc_gain(HOST_MIC_RECORD_CODEC_ID, CM_CHA_RIGHT,  DISABLE);
    cm_set_codec_adc_gain(HOST_MIC_RECORD_CODEC_ID, CM_CHA_RIGHT, 20);
    #else
    ciss_set(CI_SS_DUALMIC_IS_ANY,false);
    #endif

    extern ci_ssp_config_t ci_ssp;
    extern audio_capture_t audio_capture;
    REMOTE_CALL(set_ssp_registe(&audio_capture, (ci_ssp_st*)&ci_ssp, sizeof(ci_ssp)/sizeof(ci_ssp_st)));
    REMOTE_CALL(set_freqvad_start_para_gain(VAD_SENSITIVITY));
    ci_flash_data_info_init(DEFAULT_MODEL_GROUP_ID);
    #if USER_CODE_SWITCH_ENABLE  //支持两份code动态切换
    check_code2_param();
    #endif
    #if USE_TTS
    get_ci_tts_model_addr();
    audio_play_init();
    audio_play_set_vol_gain(85);
    sys_tts_msg_task_initial();
    tts_module_init();
    vTaskDelay(pdMS_TO_TICKS(10));
    mprintf("tts init ok\r\n");
    #else
	#if USE_BLE
    ble_model_init();
    #endif
    #if USE_CWSL 
    cwsl_set_vad_alc_config(1);
    cwsl_init();
    #endif
	
    #if USE_PWK    //就近唤醒
	cm_set_codec_alc(HOST_MIC_RECORD_CODEC_ID,CM_CHA_LEFT,DISABLE);
	cm_set_codec_adc_gain(HOST_MIC_RECORD_CODEC_ID,CM_CHA_LEFT,25);
    #endif
    #if !USE_SED          //事件检测不带asr
    extern void decoder_task_init_port(void);
    decoder_task_init_port();
    #endif
    xTaskCreate(audio_in_manage_inner_task, "audio_in_manage_inner_task", 300, NULL, 4, NULL); 
    #if AUDIO_PLAYER_ENABLE
    /* 播放器任务 */
    audio_play_init();
    #endif
    alg_model_init();     //初始化算法模型
    #if USE_AI_DOA
    xTaskCreate(doa_out_result_hand_task, "doa_out_result_hand_task", 100, NULL, 4, NULL); 
    #endif
    #if USE_SED
    sed_set_vol_init();
    sed_play_welcome_prompt();
    #endif
    #if CLOUD_UART_PROTOCOL_EN
    alg_cloud_protocol_init();
    #endif
    #if (VOICE_UPLOAD_BY_UART || VOICE_PLAY_BY_UART)
    cias_audio_data_transmission();
    #endif
    #if (MULT_INTENT > 1)
    if (nlp_module_init() != NLP_STATE_OK) //nlp模块初始化
    {
        mprintf("nlp module init error...\r\n");
        return NLP_ERR_COMMON;
    }
	nlp_msg_initial();
	xTaskCreate(nlpTaskManageProcess,"nlpTaskManageProcess",480,NULL,4,NULL);

	#endif


    /*user app初始化*/
    userapp_initial();
    /* 用户任务 */
    sys_msg_task_initial();
    xTaskCreate(UserTaskManageProcess,"UserTaskManageProcess",480,NULL,4,NULL);
    #endif
#if !NO_ASR_FLOW  
    extern void config_adpt_cnt(int enable);
    config_adpt_cnt(ADAPTIVE_CNT_ENABLE);
    extern void config_max_stop_cfd(int enable,int nocnt_max_stop_cfd,int cnt_max_stop_cfd);
    config_max_stop_cfd(MAX_STOP_CFD_ENABLE,MAX_STOP_CFD_NOCNT,MAX_STOP_CFD_CNT);
    extern void config_max_vad_end_frm(int max_vad_end_frm);
    config_max_vad_end_frm(MAX_STOP_VAD_FRM);
    extern int config_base_confidence_count(short base_confidence,unsigned char valid_count);
    config_base_confidence_count(DEFAULT_CONFIDENCE,DEFAULT_CNT);
    extern void config_recover_result(int enable,int mode,int max_frm);
    config_recover_result(RECOVER_RESULT_ENABLE,RECOVER_RESULT_MODE,RECOVER_RESULT_MAX_FRM);
    extern void config_silprob_cnt(float base_silprob,int base_silcnt );
    config_silprob_cnt(DEFAULT_STOP_SILPROB, DEFAULT_STOP_SILCNT);
#endif
    #if USER_CODE_SWITCH_ENABLE
    xTaskCreate(uart_data_handle_task,"uart_data_handle_task", 480, NULL, 4, NULL);
    #endif
    #if (!COMMAND_LINE_CONSOLE_EN)
    while(1) 
    {
        UBaseType_t ArraySize = 15;
        TaskStatus_t *StatusArray;
        //ArraySize = uxTaskGetNumberOfTasks();
        StatusArray = pvPortMalloc(ArraySize*sizeof(TaskStatus_t));
        if (StatusArray && ArraySize)
        {
            uint32_t ulTotalRunTime;
            volatile UBaseType_t ArraySize2 = uxTaskGetSystemState(StatusArray, ArraySize, &ulTotalRunTime);
            mprintf("TaskName\t\tPriority\tTaskNumber\tMinStk\t%d\n", ArraySize2);
            for (int i = 0;i < ArraySize2;i++)
            {
                mprintf("% -16s\t%d\t\t%d\t\t%d\r\n",
                    StatusArray[i].pcTaskName,
                    (int)StatusArray[i].uxCurrentPriority,
                    (int)StatusArray[i].xTaskNumber,
                    (int)StatusArray[i].usStackHighWaterMark
                );
            }
            mprintf("\n");
            extern int get_heap_bytes_remaining_size(void);
            mprintf("asr heap min free:%dKB\n", get_heap_bytes_remaining_size()/1024);
            mprintf("system heap min free:%dKB\n", xPortGetMinimumEverFreeHeapSize()/1024);
            mprintf("system heap free:%dKB\n", xPortGetFreeHeapSize()/1024);
        }
        vPortFree(StatusArray);
        vTaskDelay(pdMS_TO_TICKS(10000));
    }
    #else
    vTaskDelete(NULL);
    #endif
}

/**
 * @brief 
 * 
 */
int main(void)
{
    hardware_default_init();

    /*平台相关初始化*/
    platform_init(); 

    /* 版本信息 */
    #if !USE_BLE
    welcome();
    #endif

    /* 创建启动任务 */
    xTaskCreate(task_init, "init task", 280, NULL, 4, NULL);

    /* 启动调度，开始执行任务 */
    vTaskStartScheduler();

    while(1){}
}


