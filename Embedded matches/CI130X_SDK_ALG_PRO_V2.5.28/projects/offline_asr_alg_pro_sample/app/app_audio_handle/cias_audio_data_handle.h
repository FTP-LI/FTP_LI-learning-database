#ifndef __CIAS_AUDIO_DATA_HANDLE_H__
#define __CIAS_AUDIO_DATA_HANDLE_H__
#include "ci130x_scu.h"
#include "ci130x_uart.h"
#include "user_config.h"


#define CIAS_DOUBLE_MIC_DEMO_ENABLE                     1    //双mic demo功能-离在线
#define AUDIO_PLAY_USE_OUTSIDE                          1

#if VOICE_PLAY_BY_UART
#define IOT_AUDIO_PLAY_BUF_SIZE            (36 * 1024) // 
#define PLAY_BUF_SIZE_MAX                  (10 * 1024)  //超过此大小开始从网络获取下一帧数据
#endif


#define NETWORK_RECV_BUFF_MAX_LEN                       (1024*2)//网络端串口交互数据接收缓冲区大小
#define RECV_PCM_DATA_SIZE                              (1152*2)//(320*8)

#define NETWORK_SEND_BUFF_MAX_LEN                       (1024)   //网络端串口交互数据发送缓冲区大小
#define CIAS_STANDARD_MAGIC                             0x5a5aa5a5
#define CIAS_HEAD_MAGIC                                 0xA4

typedef struct ci_uart_standard_head
{
    uint32_t magic; /*帧头 定义为0x5a5aa5a5*/
    uint16_t checksum;  /*校验和*/
    uint16_t type;  /*命令类型*/
    uint16_t len;   /*数据有效长度*/
    uint16_t version;   /*版本信息*/
    uint32_t fill_data;     /*填充数据，可以添加私有信息*/
}cias_data_standard_head_t;

typedef enum{
    NET_MSG_HEAD = 1,
    NET_MSG_DATE = 2,
    NET_MSG_ERR  = 0,
    NET_MSG_IDE  = 3,
}cias_communicate_state_t;

int cias_audio_data_transmission(void);

#endif   //__CIAS_AUDIO_DATA_HANDLE_H__
