#include "codec_manager.h"
#include "ci130x_audio_pre_rslt_out.h"
#include <string.h>
#include "ci130x_codec.h"
#include "ci130x_dpmu.h"
#include "board.h"
#include "ci130x_gpio.h"
#include <stdbool.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "sdk_default_config.h"
#include "ci130x_uart.h"
#include "ci130x_dma.h"
#include "debug_time_consuming.h"
#include "stream_buffer.h"

#define BUFFER_NUM (4)
typedef struct
{
    audio_pre_rslt_out_init_t init_str;
    // 写了多少次数据
    uint32_t write_data_cnt;
    // 已经发送了多少次数据，写数据频次是固定的，我们不能改变，只能改变发送数据的频度，
    // 如果发送太快，会在mute模式再发送上一个buf，
    // 如果发送太慢，会扔掉一个buf不发送
    uint32_t send_data_cnt;

    int32_t write_send_sub_slave; // 在slave模式下，write和send的差值
    // 是否使用硬件tx merge功能
    bool hardware_tx_merge;
} audio_pre_init_tmp_t;

#if USE_UART_SEND_PRE_RSLT_AUDIO
StreamBufferHandle_t gUartRecordStreamBuffer = NULL; // 串口采音队列
// 串口录音任务
void uart_record_task(void *p_arg)
{
    uint16_t stream_aviable_len = 0;
    int8_t record_rx_temp[512] = {0};
    int count = 0;
    int8_t *p;
    while (1)
    {
        stream_aviable_len = xStreamBufferBytesAvailable(gUartRecordStreamBuffer);
        if (stream_aviable_len >= 512)
        {
            memset(record_rx_temp, 0, 512);
            int rx_size = xStreamBufferReceiveFromISR(gUartRecordStreamBuffer, record_rx_temp, 512, portMAX_DELAY);
            if (rx_size > 0)
            {
                audio_pre_rslt_write_data_from_uart((uint32_t)record_rx_temp, rx_size);
            }
            vTaskDelay(pdMS_TO_TICKS(15));
        }
    }
}
void uart_send_voice_init(void)
{
    UARTDMAConfig((UART_TypeDef *)USE_UART_SEND_PRE_RSLT_AUDIO_NUMBER, USE_UART_SEND_PRE_RSLT_AUDIO_BAUD);
    extern StreamBufferHandle_t gUartRecordStreamBuffer;
    gUartRecordStreamBuffer = xStreamBufferCreate(1024 * 10, 1);
    xTaskCreate(uart_record_task, "uart_record_task", 1024, NULL, 4, NULL);
}
#endif


volatile uint8_t uart_dma_trans_done = 0;

static void uart_dma_read_irq_callback(void)
{
    uart_dma_trans_done = 1;
}

void audio_pre_rslt_write_data_from_uart(uint32_t addr, uint32_t size)
{
#if USE_UART_SEND_PRE_RSLT_AUDIO
    // init_timer0();
    // timer0_start_count();
    if (!uart_dma_trans_done)
    {
        // mprintf("pre voice data overflow\n");
    }
    uart_dma_trans_done = 0;
    extern void set_dma_int_callback(DMACChannelx dmachannel, dma_callback_func_ptr_t func);
    set_dma_int_callback(DMACChannel1, uart_dma_read_irq_callback);

    DMAC_Peripherals uart_dma_peripherals_num = DMAC_Peripherals_UART0_TX;
    uint32_t uart_fifo_addr = UART0FIFO_BASE;
    if (((uint32_t)USE_UART_SEND_PRE_RSLT_AUDIO_NUMBER == (uint32_t)UART1))
    {
        uart_dma_peripherals_num = DMAC_Peripherals_UART1_TX;
        uart_fifo_addr = UART1FIFO_BASE;
    }
    else if (((uint32_t)USE_UART_SEND_PRE_RSLT_AUDIO_NUMBER == (uint32_t)UART2))
    {
        uart_fifo_addr = UART2FIFO_BASE;
        uart_dma_peripherals_num = DMAC_Peripherals_UART2_TX;
    }

    DMAC_M2P_P2M_advance_config(DMACChannel1, uart_dma_peripherals_num, M2P_DMA, addr, uart_fifo_addr, size,
                                TRANSFERWIDTH_8b, BURSTSIZE1, DMAC_AHBMaster1);
#endif
    // while(!uart_dma_trans_done);
    // uart_dma_trans_done = 0;
    // timer0_end_count_only_print_time_us();
}

static audio_pre_init_tmp_t sg_init_tmp_str;

#define PI (3.1416926f)
void sine_wave_generate(int16_t *sine_wave, uint32_t sample_rate, uint32_t wave_fre, uint32_t point_num)
{
    int16_t num_of_one_period = sample_rate / wave_fre; // 每个周期的采样点数
    for (int i = 0; i < point_num; i++)
    {
        sine_wave[i] = (int16_t)(32767.0f * sinf((2 * PI * i) / num_of_one_period));
    }
}

uint32_t tmp_voice_addr = 0;

void audio_pre_rslt_out_play_card_init(void)
{
    uint16_t block_size = AUDIO_CAP_POINT_NUM_PER_FRM * 2 * sizeof(int16_t);

    sg_init_tmp_str.init_str.block_size = block_size;
    sg_init_tmp_str.write_data_cnt = 0;
    sg_init_tmp_str.send_data_cnt = 0;

#if USE_IIS1_OUT_PRE_RSLT_AUDIO || USE_HP_OUT_PRE_RSLT_AUDIO || USE_UART_SEND_PRE_RSLT_AUDIO
#if USE_UART_SEND_PRE_RSLT_AUDIO
    uart_send_voice_init();
    tmp_voice_addr = pvPortMalloc(block_size);
    CI_ASSERT(tmp_voice_addr, "\n");
#else
#if USE_IIS1_OUT_PRE_RSLT_AUDIO
    audio_pre_rslt_out_codec_init();
#endif

#if USE_HP_OUT_PRE_RSLT_AUDIO
    audio_pre_rslt_out_codec_init_pa_out();
#endif
#endif
#endif
}

#if USE_DENOISE_NN_RTC
static bool is_convert_left_right = false;
void convert_left_and_right(bool r)
{
    is_convert_left_right = r;
}
#endif

/**
 * @brief 写数据到发送端
 *
 * @param rslt 处理结果的起始指针
 * @param origin 原始数据的起始指针
 *  此函数不可用于中断
 */
void audio_pre_rslt_write_data(int16_t *left, int16_t *right)
{

    uint32_t block_size = sg_init_tmp_str.init_str.block_size;

#if USE_IIS1_OUT_PRE_RSLT_AUDIO || USE_HP_OUT_PRE_RSLT_AUDIO || USE_UART_SEND_PRE_RSLT_AUDIO
#if USE_UART_SEND_PRE_RSLT_AUDIO
    int16_t *pcm_data_p = (int16_t *)tmp_voice_addr;
    int num = block_size / sizeof(int16_t) / 2;
    // for (int i = 0; i < num; i++)
    // {
    //     pcm_data_p[2 * i] = left[i];
    //     pcm_data_p[2 * i + 1] = right[i];
    // }
    static int cnt = 0;
    cnt++;
    if (cnt == 30)
    {
        uart_send_voice_init();
    }
    else if (cnt >= 30)
    {
        // audio_pre_rslt_write_data_from_uart((uint32_t)tmp_voice_addr,block_size);
        #if USE_UART_SEND_PRE_RSLT_RECORD_LEFT
        int8_t *src_addr = (int8_t *)left;
        #else
        int8_t *src_addr = (int8_t *)right;

        #endif
        if (gUartRecordStreamBuffer)
        {
            int ret = xStreamBufferSendFromISR(gUartRecordStreamBuffer, src_addr, block_size / 2, 15);
            if (ret != block_size / 2)
            {
                mprintf("xSpeexRecordStreamBuffer send error, send len = %d\r\n", ret);
            }
        }
    }
#else
    int num = block_size / sizeof(int16_t) / 2;
#if USE_IIS1_OUT_PRE_RSLT_AUDIO
    uint32_t write_pcm_addr = 0;
    cm_get_pcm_buffer(PLAY_PRE_AUDIO_CODEC_ID, &write_pcm_addr, 0); // TODO HSL
    int16_t *pcm_data_p = (int16_t *)write_pcm_addr;
    for (int i = 0; i < num; i++)
    {
#if USE_DENOISE_NN_RTC
        if (is_convert_left_right)
        {
            pcm_data_p[2 * i] = right[i];
            pcm_data_p[2 * i + 1] = left[i];
        }
        else
#endif
        {
            pcm_data_p[2 * i] = left[i];
            pcm_data_p[2 * i + 1] = right[i];
        }
    }
    if (0 == write_pcm_addr)
    {
        return;
    }
    cm_write_codec(PLAY_PRE_AUDIO_CODEC_ID, (void *)write_pcm_addr, 0);
#endif

#if USE_HP_OUT_PRE_RSLT_AUDIO
    uint32_t write_pcm_addr_cpy = 0;
    cm_get_pcm_buffer(PLAY_CODEC_ID, &write_pcm_addr_cpy, 0); // TODO HSL
    int16_t *pcm_data_p_cpy = (int16_t *)write_pcm_addr_cpy;
    for (int i = 0; i < num; i++)
    {
#if USE_DENOISE_NN_RTC
        if (is_convert_left_right)
        {
            pcm_data_p_cpy[2 * i] = right[i];
            pcm_data_p_cpy[2 * i + 1] = left[i];
        }
        else
#endif
        {
            pcm_data_p_cpy[2 * i] = left[i];
            pcm_data_p_cpy[2 * i + 1] = right[i];
        }
    }
    if (0 == write_pcm_addr_cpy)
    {
        return;
    }
    cm_write_codec(PLAY_CODEC_ID, (void *)write_pcm_addr_cpy, 0);
#endif

    if (0 == sg_init_tmp_str.send_data_cnt)
    {
#if USE_IIS1_OUT_PRE_RSLT_AUDIO
        cm_start_codec(PLAY_PRE_AUDIO_CODEC_ID, CODEC_OUTPUT);
#endif
#if USE_HP_OUT_PRE_RSLT_AUDIO
        cm_start_codec(PLAY_CODEC_ID, CODEC_OUTPUT);
#endif
    }
#endif
#endif

    sg_init_tmp_str.send_data_cnt++;

    sg_init_tmp_str.write_data_cnt++;
}

/**
 * @brief 语音前处理输出停止
 *
 */
void audio_pre_rslt_stop(void)
{
#if USE_IIS1_OUT_PRE_RSLT_AUDIO || USE_HP_OUT_PRE_RSLT_AUDIO
#if USE_UART_SEND_PRE_RSLT_AUDIO
#else

#if USE_IIS1_OUT_PRE_RSLT_AUDIO
    cm_stop_codec(PLAY_PRE_AUDIO_CODEC_ID, CODEC_OUTPUT);
#endif

#if USE_HP_OUT_PRE_RSLT_AUDIO
    cm_stop_codec(PLAY_CODEC_ID, CODEC_OUTPUT);
#endif
#endif
#endif
}

/**
 * @brief 语音前处理输出开始
 *
 */
void audio_pre_rslt_start(void)
{
#if USE_IIS1_OUT_PRE_RSLT_AUDIO || USE_HP_OUT_PRE_RSLT_AUDIO
#if USE_UART_SEND_PRE_RSLT_AUDIO
#else

#if USE_IIS1_OUT_PRE_RSLT_AUDIO
    cm_start_codec(PLAY_PRE_AUDIO_CODEC_ID, CODEC_OUTPUT);
#endif
#if USE_HP_OUT_PRE_RSLT_AUDIO
    cm_start_codec(PLAY_CODEC_ID, CODEC_OUTPUT);
#endif
#endif
#endif
}

/********** (C) COPYRIGHT Chipintelli Technology Co., Ltd. *****END OF FILE****/
