#ifndef __CODE_SWITCH_C__
#define __CODE_SWITCH_C__

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

#define DNN_CODE2_ADDR_ID 60099 
uint32_t get_code2_addr(int index);
void uart_data_handle_task(void);
void check_code2_param(void);

typedef enum
{
    ASR_SWITCH_TO_TTS,
    TTS_SWITCH_TO_ASR,
    ASR_SWITCH_TO_RTC,
    RTC_SWITCH_TO_ASR
}code_switch_type_t;
#endif