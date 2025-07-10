/*
 * @Description: file Description
 * @Date: 2020-08-11 15:40:58
 * @LastEditTime: 2020-09-03 10:53:00
 * @FilePath: \xiaobao_2nd\sample\internal\ir_common\LinkMsgProc\ir_uart_msg_deal.h
 * @copyright Copyright: (c) 2020  Chipintelli Technology Co., Ltd.
 */
#ifndef __IR_UART_MSG_DEAL_H__
#define __IR_UART_MSG_DEAL_H__

#include "ir_data.h"
#include "ci130x_uart.h"

#ifdef USE_UART_COMMUNIT

#define CI_UART_STANDARD_MAGIC                  (0x5a5aa5a5)     //打包数据包头

#define BROADLINK_BUF_SIZE                      (4*1024)        //打包缓存区
#define MAX_FRAME_BUF_SIZE                      (2048)           //最大打包长度

/*设置串口号*/
#define IR_UART_PROTOCOL_NUMBER  (HAL_UART1_BASE)
/*设置波特率*/
#define IR_UART_PROTOCOL_BAUDRATE (UART_BaudRate115200)

/**
 * @brief unsigned short
 * 如果需要发送电平数据，input 数据需要为 ushort 类型数组数据，
 * 且原始电平数据需要除以 IR_DATA_DIV_COEFFICIENT 系数，目前为2
 */
typedef enum
{
    IR_CMD_RECEIVE_START = 0x1000, //开始接收数据
    IR_CMD_RECEIVE_STOP,           //结束接收
    IR_CMD_LEVEL_SEND,             //发送电平数据
    IR_CMD_LEVEL_MATCH,            //匹配电平数据
    IR_CMD_LEVEL_LEARN,            //学习电平数据
    IR_CMD_DEV_RECV_IR_DATA,       //设备接收到的红外数据
    IR_CMD_DEV_MATCH_OK,           //设备匹配成功，设备返回数据，fill_data为匹配ID
    IR_CMD_DEV_MATCH_ERR,          //设备匹配失败，设备返回匹配失败
    IR_CMD_SET_AIR_CODE_ID,        //设置空调红外码ID，ID号放到fill_data
    IR_CMD_ACK = 0xFAFA,           //ACK 回复信息
} ir_uart_cmd_t;



typedef struct uart_standard_head
{
    unsigned int magic; /*帧头 定义为0x5a5aa5a5*/
    unsigned short checksum;  /*校验和*/
    unsigned short type;  /*命令类型*/
    unsigned short len;   /*数据有效长度*/
    unsigned short version;   /*版本信息*/
    unsigned int fill_data;     /*填充数据，可以添加私有信息*/
}uart_standard_head_t;


/**
 * @brief IR 串口通信初始化
 *
 * @return int -1 错误， 0 成功
 */
int ir_uart_buff_init(void);


/**
 * @brief 发送消息到设备端
 *
 * @param msg_type 消息类型
 * @param input 数据包
 * @param len 数据包长度
 * @param fill_data 填充信息
 * @return int -1 错误， 返回发送数据长度
 */
int send_server_data_to_device(ir_uart_cmd_t msg_type, const unsigned char *input, int len,unsigned int fill_data);


/**
 * @brief 串口数据解析
 *
 * @param char 串口数据
 * @param data_len 数据长度
 * @return int 0 解析成功，-1 解析错误
 */
int uart_data_decode(unsigned char* uart_data, int data_len);


#endif
#endif
