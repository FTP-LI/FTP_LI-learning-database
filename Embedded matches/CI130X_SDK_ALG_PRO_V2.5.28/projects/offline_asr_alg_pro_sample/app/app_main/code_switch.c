#include "code_switch.h"

#if USER_CODE_SWITCH_ENABLE
uint32_t get_code2_addr(int index)
{
    uint32_t addr = 0;
    uint32_t size = 0;
    uint8_t code2_info[8] = {0};
    uint8_t aes_key[32] = {0};    //aes 秘钥32字节存放在0x30000地址，需要先备份
    post_read_flash(aes_key, 0x3000, 32);
    if (index == ASR_SWITCH_TO_TTS) // asr 切 tts
    {
        mprintf("asr switch to tts\r\n");
        if (get_dnn_addr_by_id(DNN_CODE2_ADDR_ID, &addr, &size))
        {
            mprintf("code2 addr = 0x%x, size = %d\r\n", addr, size);
        }
        else
        {
            mprintf("DNN_CODE2_ADDR_ID get error...\r\n");
        }
        uint8_t code_info[8] = {0};
        code_info[0] = (addr >> 24);
        code_info[1] = (addr >> 16);
        code_info[2] = (addr >> 8);
        code_info[3] = addr;

        code_info[4] = (size >> 24);
        code_info[5] = (size >> 16);
        code_info[6] = (size >> 8);
        code_info[7] = size;
        mprintf("-----------code_info addr-----------\r\n");
        for (int i = 0; i < 8; i++)
        {
            mprintf("%02x ", code_info[i]);
        }
        post_erase_flash(0x3000, 0x1000);
        post_write_flash(aes_key, 0x3000, 32);   //还原秘钥数据
        post_write_flash(code_info, USER_CODE2_SAVE_ADDR, 8);
        mprintf("-----------code_info end-----------\r\n");
    }
    else if(index == TTS_SWITCH_TO_ASR)  //tts切换asr
    {
        mprintf("tts switch to asr, code addr = 0x40000\r\n");
        addr = 0x4000;   //默认在0x4000地址
        uint8_t code_info[8] = {0};
        code_info[0] = (addr >> 24);
        code_info[1] = (addr >> 16);
        code_info[2] = (addr >> 8);
        code_info[3] = addr;

        code_info[4] = (size >> 24);
        code_info[5] = (size >> 16);
        code_info[6] = (size >> 8);
        code_info[7] = size;
        mprintf("-----------code_info addr-----------\r\n");
        for (int i = 0; i < 8; i++)
        {
            mprintf("%02x ", code_info[i]);
        }
        post_erase_flash(0x3000, 0x1000);
        post_write_flash(aes_key, 0x3000, 32);  //还原秘钥数据
        post_write_flash(code_info, USER_CODE2_SAVE_ADDR, 8);
        mprintf("-----------code_info end-----------\r\n");
    }
    mprintf("sys reset ...\r\n");
    _delay_10us_240M(10000);
    dpmu_software_reset_system_config();
}
void check_code2_param(void)
{
    uint8_t code2_info[8] = {0};
    post_read_flash(code2_info, USER_CODE2_SAVE_ADDR, 8);   //code2固件地址和大小写在0x4000-8位置
    int index = 0;
    for(int i = 0; i < code2_info[i]; i++)
    {
        if(code2_info[i] == 0xFF)
        {
            index++;
        }
    }
    if(index == 8)   //USER_CODE2_SAVE_ADDR地址为初始化状态
    {
        get_code2_addr(TTS_SWITCH_TO_ASR);
    }
}
QueueHandle_t uart_msg_recv_queue = NULL;
void uart_data_handle_task(void)
{
    uart_msg_recv_queue = xQueueCreate(5, 8);
    uint8_t recv_uart_buf[8] = {0};
     while (1)
    {
        memset(recv_uart_buf, 0, 8);
        xQueueReceive(uart_msg_recv_queue, recv_uart_buf, portMAX_DELAY);
        for(int i = 0; i < 8; i++)
        {
            mprintf("%02x ", recv_uart_buf[i]);
        }
        if((recv_uart_buf[0] == 0xA5) && recv_uart_buf[1] == 0xFC)  // asr状态切tts
        {
            if(recv_uart_buf[5] == 0x23)
            {
                mprintf("asr switch to tts\r\n");
                get_code2_addr(ASR_SWITCH_TO_TTS);
            }
        }
        if(recv_uart_buf[0] == 0xfd)                //tts状态切换asr
        {
            if(recv_uart_buf[3] == 0x22)
            {
                mprintf("tts switch to asr\r\n");
                get_code2_addr(TTS_SWITCH_TO_ASR);
            }
            
        }
    }
}
#endif
