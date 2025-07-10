/*
 * @FileName::
 * @Author: 
 * @Date: 2022-03-04 10:41:12
 * @LastEditTime: 2022-05-17 20:16:17
 * @Description: Network driven implementation 主要用于和wifi芯片通讯
 */
#include <stdint.h>
#include <string.h>
#include "sdk_default_config.h"
#include "ci130x_uart.h"
#include "ci130x_dma.h"
#include "cias_uart_protocol.h"
#include "system_msg_deal.h"
#include "ci_log.h"
#include "cias_audio_data_handle.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "codec_manager.h"
#include "audio_play_process.h"
#include "audio_play_device.h"
#include "audio_play_api.h"

#if VOICE_PLAY_BY_UART || VOICE_UPLOAD_BY_UART
#define  AUDIO_RCV_FRAME_SIZE      1 
int8_t   g_audio_rx_temp_buf[AUDIO_RCV_FRAME_SIZE] = {0};         // 串口接收数据缓存buf
uint16_t g_audio_rx_temp_index = 0;                               // 串口接收音频数据buf 索引
QueueHandle_t gUartRcvSpeexDataQueue = NULL;

extern audio_play_os_stream_t mp3_player;
extern audio_play_os_sem_t mp3_player_end;
volatile int cias_wakeup_cmd_flag = 1;



void UART1_IRQHandler(void)
{
    UART_TypeDef *uart = (UART_TypeDef *)UART_NUM_SEND_PLAY_AUDIO_NUMBER;
    /*接受数据*/
    if ((uart->UARTMIS & (1UL << UART_RXInt)) || (uart->UARTMIS & (1UL << UART_RXTimeoutInt)))
    {
        while (!(uart->UARTFlag & (0x1 << 6))) // read fifo have data
        {
            if(gUartRcvSpeexDataQueue != NULL)
            {
                g_audio_rx_temp_buf[g_audio_rx_temp_index++] = (int8_t)UartPollingReceiveData(UART_NUM_SEND_PLAY_AUDIO_NUMBER);
                if (g_audio_rx_temp_index == AUDIO_RCV_FRAME_SIZE)
                {
                    g_audio_rx_temp_index = 0;
                    if(xQueueSendFromISR(gUartRcvSpeexDataQueue, g_audio_rx_temp_buf, pdFALSE) == pdFALSE)
                        mprintf("----queue send err------\n");
                }
            }
        }
        UART_IntClear(uart, UART_RXInt);
    }
    uart->UARTICR = 0xFFF;
    
}

int network_recv_data_task(void)
{
    uint8_t recv_pcm_buf[AUDIO_RCV_FRAME_SIZE] = {0};
    gUartRcvSpeexDataQueue = xQueueCreate(1024*2, AUDIO_RCV_FRAME_SIZE);
    while(1)
    {
        if(xQueueReceive(gUartRcvSpeexDataQueue, recv_pcm_buf, pdMS_TO_TICKS(500)) == pdPASS)
        {
          //  mprintf("start play....\r\n");
            if(cias_wakeup_cmd_flag == 1)
            {
                cias_wakeup_cmd_flag = 0;
                set_curr_outside_handle(mp3_player, mp3_player_end);
                outside_clear_stream(mp3_player, mp3_player_end);
            }
            outside_write_stream(mp3_player, recv_pcm_buf, 1, false);
            play_with_outside(0, "mp3", NULL); 
        }
        else
            outside_send_end_sem(mp3_player_end);
    }
}


static int8_t network_send_Byte(int8_t ch)
{
    //发送FIFO满标志
    if (UART_FLAGSTAT(UART_NUM_SEND_PLAY_AUDIO_NUMBER, UART_TXFF))
    {
        return 0;
    }
    UartPollingSenddata(UART_NUM_SEND_PLAY_AUDIO_NUMBER, ch);

    return 1;
}

int32_t network_send(int8_t *str, uint32_t length)
{
    int32_t ret = 0;

    if (str && length)
    {
        while (length && (ret < NETWORK_SEND_BUFF_MAX_LEN))
        {
            if (network_send_Byte(str[ret]))
            {
                ret++;
                length--;
            }
        }
    }
    return ret;
}

/** 网络通讯初始化
 *  入口参数：void
 *  return： 0
*/
int8_t network_uart_port_init(void)
{
    UARTInterruptConfig(UART_NUM_SEND_PLAY_AUDIO_NUMBER, UART_NUM_SEND_PLAY_AUDIO_BAUDRATE);
    return 0;
}
#endif