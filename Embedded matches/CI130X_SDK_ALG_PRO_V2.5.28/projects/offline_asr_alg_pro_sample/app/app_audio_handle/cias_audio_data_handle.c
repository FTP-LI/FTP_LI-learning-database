#include "cias_audio_data_handle.h"
#include "ci_log.h"
#include "FreeRTOS.h"
#include "queue.h"
#include <stdint.h>
#include <string.h>
#include "cias_network_msg_send_task.h"
#include "semphr.h"
#include "cias_network_msg_protocol.h"
#include "audio_play_api.h"
#include "codec_manager.h"
#include "ci_assert.h"

int cias_audio_data_transmission(void)
{
    #if VOICE_PLAY_BY_UART || VOICE_UPLOAD_BY_UART
    network_uart_port_init();
    #endif
    #if VOICE_PLAY_BY_UART
    extern void audio_player_param_init();
    audio_player_param_init();
    xTaskCreate(network_recv_data_task, "rcv_uart_audio_play", 256,NULL, 4, NULL);   //通过串口播放音频任务
    #endif

    #if VOICE_UPLOAD_BY_UART
    xTaskCreate(network_send_data_task, "send_uart_audio", 256, NULL, 4, NULL);       //通过串口上传语音任务
    voice_upload_task_init();
    #endif
}