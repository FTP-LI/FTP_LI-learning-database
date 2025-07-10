#include "ir_uart_msg_deal.h"
#include "LinkMsgProc.h"
#include "ci_log.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include "ir_data.h"

#ifdef USE_UART_COMMUNIT

extern void set_air_device_match_id(int ir_code);

/*调试打印*/
#define IR_UART_DEBUG(fmt,...) mprintf(fmt"\r\n",##__VA_ARGS__)

static unsigned char *uart_decode_buffer = NULL;               //UART解码使用BUFF，申请2KB
static unsigned char *broad_link_buf = NULL;                    //16KB
static unsigned char *link_msg_buf = NULL;                      //16KB


/* 需要修改的接口 */
static int write_device_data_interface(unsigned char *file_data, int file_lenth);
static void *ir_malloc(unsigned int want_size);
static int ir_uart_port_hw_init(void);




/**
 * @brief 内存分配
 *
 * @param want_size 申请空间
 * @return void* 返回内存地址
 */
static void *ir_malloc(unsigned int want_size)
{
    return pvPortMalloc(want_size);  //FREERTOS 内存申请 CI1103 测试使用
}


/**
 * @brief 串口发送数据
 *
 * @param ch 数据
 * @return int 0 OK，-1 Fail
 */
static int ir_uart_port_send_char(uint8_t ch)
{
    #if (IR_UART_PROTOCOL_NUMBER == HAL_UART0_BASE)
    UartPollingSenddata(UART0,ch);
    #endif

    #if (IR_UART_PROTOCOL_NUMBER == HAL_UART1_BASE)
    UartPollingSenddata(UART1,ch);
    #endif

    return 0;
}


/**
 * @brief 串口初始化
 *
 * @return int 0 OK，-1 Fail
 */
static int ir_uart_port_hw_init(void)
{
    #if (IR_UART_PROTOCOL_NUMBER == HAL_UART0_BASE)
    UARTInterruptConfig(UART0,IR_UART_PROTOCOL_BAUDRATE);
    #endif

    #if (IR_UART_PROTOCOL_NUMBER == HAL_UART1_BASE)
    UARTInterruptConfig(UART1,IR_UART_PROTOCOL_BAUDRATE);
    #endif

    return 0;
}


/**
 * @brief IR 串口通信初始化
 *
 * @return int -1 错误， 0 成功
 */
int ir_uart_buff_init(void)
{
    ir_uart_port_hw_init();

    broad_link_buf = (unsigned char *)ir_malloc(BROADLINK_BUF_SIZE);
    if(broad_link_buf == NULL)
    {
        IR_UART_DEBUG("broad_link_buf alloc error");
        return -1;
    }

    link_msg_buf = (unsigned char *)ir_malloc(BROADLINK_BUF_SIZE);
    if(link_msg_buf == NULL)
    {
        IR_UART_DEBUG("link_msg_buf alloc error");
        return -1;
    }

    uart_decode_buffer = (unsigned char *)ir_malloc(MAX_FRAME_BUF_SIZE);
    if(uart_decode_buffer == NULL)
    {
        IR_UART_DEBUG("uart decode buf alloc error");
        return -1;
    }

    return 0;
}


/**
 * @brief 实现数据写入设备接口
 *
 * @param file_data 数据
 * @param file_lenth 数据长度
 * @return int -1 错误，其他 数据长度
 */
static int write_device_data_interface(unsigned char *file_data, int file_lenth)
{
    /*需要根据实际情况实现*/

    /*使用UART1 波特率921600 发送数据*/

    for(int i = 0; i < file_lenth; i++)
    {
        ir_uart_port_send_char(file_data[i]);
    }
    return file_lenth;

}


/**
 * @brief 数据打包
 *
 * @param type 数据类型
 * @param input 输入数据
 * @param len  输入数据长度
 * @param output 打包输出buf
 * @param maxsize 最大长度
 * @param fill_data 填充数据
 * @param index 数据编号
 * @return int -1 错误，返回数据包长度
 */
static int msg_frame_create(ir_uart_cmd_t type, const unsigned char *input, unsigned int len,unsigned char *output, unsigned int maxsize,unsigned int fill_data,unsigned short index)
{
    uart_standard_head_t *head = (uart_standard_head_t *)output;

    /* Check param */
    if (output == NULL || len + sizeof(uart_standard_head_t) > maxsize)
    {
        return -1;
    }
    /* DNA standard header initialization */
    memset(head, 0, sizeof(uart_standard_head_t));
    head->magic = CI_UART_STANDARD_MAGIC;
    head->type = (unsigned short)type;
    head->len = len;
    head->version = index;

    head->fill_data = fill_data;
    /* Fill data part */
    if((input) && (len > 0))
    memcpy(output + sizeof(uart_standard_head_t), input, len);

    return head->len + sizeof(uart_standard_head_t);
}



/**
 * @brief 发送消息到设备端
 *
 * @param msg_type 消息类型
 * @param input 数据包
 * @param len 数据包长度
 * @param fill_data 填充信息
 * @return int -1 错误， 返回发送数据长度
 */
int send_server_data_to_device(ir_uart_cmd_t msg_type, const unsigned char *input, int len,unsigned int fill_data)
{
    if((len < 0))
    {
        return -1;
    }

    /*数据打包*/
    static int buff_count = 0;
    int package_length = 0;
    unsigned char *frame_out_buf = NULL;
    unsigned char *link_out_buf = NULL;

    buff_count++;
    if(buff_count >= (BROADLINK_BUF_SIZE/MAX_FRAME_BUF_SIZE))
    {
        buff_count = 0;
    }
    /*打包到不同地址，防止数据覆盖*/

    /*添加数据包头*/
    frame_out_buf = broad_link_buf + buff_count * MAX_FRAME_BUF_SIZE;
    package_length = msg_frame_create(msg_type,input,len,frame_out_buf,MAX_FRAME_BUF_SIZE,fill_data,0);

    /*数据编码*/
    link_out_buf = link_msg_buf + buff_count * MAX_FRAME_BUF_SIZE;
    package_length = LinkMsgCode(frame_out_buf, package_length, link_out_buf, MAX_FRAME_BUF_SIZE );

    /*对齐处理*/
    if ((package_length % 16) != 0)
    {
        package_length += (16 - (package_length % 16));
    }

    /*数据发送*/
    write_device_data_interface(link_out_buf,package_length);

    return package_length;
}




/**
 * @brief 串口数据解析
 *
 * @param char 串口数据
 * @param data_len 数据长度
 * @return int 0 解析成功，-1 解析错误
 */
int uart_data_decode(unsigned char* uart_data, int data_len)
{
    static DeCodeInfo decode_status;
    int package_length = 0;

    for (int i = 0; i < data_len; i++)
    {
        /*解析数据包*/
        package_length = LinkMsgDeCode(uart_data, (unsigned char *)uart_decode_buffer, MAX_FRAME_BUF_SIZE-16, &decode_status);
        uart_data++;
        if(package_length > 0)
        {
            decode_status.bcc = 0;
            decode_status.inLen = 0;
            decode_status.status = 0;
            /* 接受消息处理 */
            uart_standard_head_t * pheader = (uart_standard_head_t *)(uart_decode_buffer + 1);

            ir_uart_cmd_t msg_type = (ir_uart_cmd_t)pheader->type;

            unsigned char* receive_data = (unsigned char *)(uart_decode_buffer + 17);
            int receive_data_len = pheader->len;
            int fill_data = pheader->fill_data;

            IR_UART_DEBUG("msg_type = 0x%x  receive_data_len = %d  fill_data = 0x%x",msg_type,receive_data_len,fill_data);

            /* 回复ACK数据 */
            if(msg_type != IR_CMD_ACK)
            {
                send_server_data_to_device(IR_CMD_ACK,NULL,0,(unsigned int)msg_type);
            }

            switch (msg_type)
            {
                /*开始接收数据*/
                case IR_CMD_RECEIVE_START:
                {
                    IR_UART_DEBUG("IR_CMD_RECEIVE_START");
                    ir_data_learn_ctl(IR_START_RECEIVE_LEVEL,NULL);
                }
                break;

                /*结束接收*/
                case IR_CMD_RECEIVE_STOP:
                {
                    IR_UART_DEBUG("IR_CMD_RECEIVE_STOP");
                    ir_data_learn_ctl(IR_STOP_RECEIVE_LEVEL,NULL);
                }
                break;

                /*发送电平数据*/
                case IR_CMD_LEVEL_SEND:
                {
                    IR_UART_DEBUG("IR_CMD_LEVEL_SEND");
                    send_ir_level((unsigned short *)receive_data, receive_data_len/2);
                }
                break;

                /*匹配电平数据*/
                case IR_CMD_LEVEL_MATCH:
                {
                    IR_UART_DEBUG("IR_CMD_LEVEL_MATCH");
                    send_msg_to_ir_task(IR_START_AIR_KEY_MATCH_FROM_UART,(unsigned short *)receive_data,
                    receive_data_len/2, pheader->fill_data);
                }
                break;

                /*学习电平数据*/
                case IR_CMD_LEVEL_LEARN:
                {
                    IR_UART_DEBUG("IR_CMD_LEVEL_LEARN");
                    send_msg_to_ir_task(IR_START_KEY_LEARN_FROM_UART,(unsigned short *)receive_data,
                    receive_data_len/2, pheader->fill_data);
                }
                break;

                /*设置空调红外码ID*/
                case IR_CMD_SET_AIR_CODE_ID:
                {
                    IR_UART_DEBUG("IR_CMD_SET_AIR_CODE_ID");
                    set_air_device_match_id(fill_data);
                }
                break;


                /* 接收到ACK */
                case IR_CMD_ACK:
                {
                    IR_UART_DEBUG("IR_CMD_ACK");
                    break;
                }

                default:
                    IR_UART_DEBUG("not deal");
                    break;
            }

            package_length = 0;

        }
        else if(LINK_DECODE_RET_ERROR == package_length)
        {
            decode_status.bcc = 0;
            decode_status.inLen = 0;
            decode_status.status = 0;
            package_length = 0;

            IR_UART_DEBUG("uart data decode err");
            return -1;
        }
    }
    return 0;
}

#endif
