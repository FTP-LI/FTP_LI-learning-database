#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "FreeRTOS.h"
#include "timers.h"
#include "ci130x_uart.h"
#include "command_info.h"
#include "sdk_default_config.h"
#include "system_msg_deal.h"

#ifndef _VOICE_MODULE_UART_TTS_PROTOCOL_
#define _VOICE_MODULE_UART_TTS_PROTOCOL_

#ifdef __cplusplus
extern "C"
{
#endif


#define PACKET_MIN_SIZE (40 * 2 + 12)
#define VMUP_MSG_DATA_MAX_SIZE (PACKET_MIN_SIZE + 6)

#pragma pack(1)
typedef struct
{
    uint16_t header;
    uint16_t data_length;
    uint8_t msg_cmd;
    uint8_t msg_type;
    // uint8_t msg_seq;
    uint8_t msg_data[VMUP_MSG_DATA_MAX_SIZE];
    /*uint16_t chksum; send add auto*/
    /*uint8_t tail; send add auto*/
} sys_tts_msg_com_data_t;
#pragma pack()
typedef struct
{
    sys_msg_type_t msg_type;/*here will be modify use union*/
    union
    {
        sys_tts_msg_com_data_t com_data;
    }msg_data;
}sys_tts_msg_t;
/*header*/
#define VMUP_TTS_MSG_HEAD_LOW (0xFD)
#define VMUP_TTS_MSG_HEAD_HIGH (0x00)
#define VMUP_TTS_MSG_HEAD ((VMUP_TTS_MSG_HEAD_HIGH << 8) | VMUP_TTS_MSG_HEAD_LOW)

extern TimerHandle_t timer_receive_data_timeout;
void timer_receive_data_timeout_callback(xTimerHandle xTimer);
extern const char tts_ring[];
extern const char tts_message[];
extern const char tts_alert[];
extern const char tts_sound[];
extern const char tts_defult_vst[];
// #define VMUP_TTS_MSG_HEAD_LOW  (0xA5)
// #define VMUP_TTS_MSG_HEAD_HIGH (0xFC)
// #define VMUP_TTS_MSG_HEAD   ((VMUP_TTS_MSG_HEAD_HIGH<<8)|VMUP_TTS_MSG_HEAD_LOW)

/*tail*/
#define VMUP_MSG_TAIL (0xFB)

/*msg_type*/
// tts

//#define VMUP_MSG_TYPE_VST_LEGTH (0x04)//语速语调音量帧长度
#define VMUP_TTS_MSG_TYPE_DEFAULT_VST_LEGTH (0x10)//语速语调音量恢复出厂默认值，帧长度

#define VMUP_TTS_MSG_TYPE_RING_LEGTH (0x06)//
#define VMUP_TTS_MSG_TYPE_MESSAGE_LEGTH (0x09)//
#define VMUP_TTS_MSG_TYPE_ALERT_LEGTH (0x07)//

#define VMUP_MSG_TYPE_GB2312 (0x00)
#define VMUP_MSG_TYPE_GBK (0x01)
#define VMUP_MSG_TYPE_BIG5 (0x02)
#define VMUP_MSG_TYPE_TF16LE (0x03)
#define VMUP_MSG_TYPE_UTF8 (0x04)
// tts

/*msg_cmd*/
// tts
#define VMUP_TTS_MSG_CMD_START (0x01)  //开始命令
#define VMUP_TTS_MSG_CMD_STOP (0x02)   //停止命令
#define VMUP_TTS_MSG_CMD_PAUSE (0x03)  //暂停命令
#define VMUP_TTS_MSG_CMD_RESUME (0x04) //继续命令
#define VMUP_TTS_MSG_CMD_STATUS (0x21)     //查询命令
#define VMUP_TTS_CHANGE_ASR_STATUS (0x22)     //tts切换到asr状态

#define VMUP_TTS_MSG_CMD_FINISH_INIT (0x4A) //完成初始化
#define VMUP_TTS_MSG_CMD_BUSY (0x4E)               //正忙
#define VMUP_TTS_MSG_CMD_IDLE (0x4F)               //空闲
#define VMUP_TTS_MSG_CMD_RECEIVE_SUCCESS (0x41)        //收到错误的命令帧
#define VMUP_TTS_MSG_CMD_RECEIVE_ERR (0x45)        //收到错误的命令帧
#define VMUP_TTS_MSG_CMD_RECEIVE_INCOMPLETE (0x46) //收到的命令帧数据不完整，或者帧数据

void vmup_tts_receive_packet(uint8_t receive_char);
void vmup_tts_communicate_init(void);
int vmup_port_send_packet_rev_text_msg(sys_tts_msg_com_data_t *msg);
int vmup_port_send_packet_rev_cmd_msg(sys_tts_msg_com_data_t *msg);
void vmup_tts_send_receive_ack(uint8_t status);
#ifdef __cplusplus
}
#endif

#endif
