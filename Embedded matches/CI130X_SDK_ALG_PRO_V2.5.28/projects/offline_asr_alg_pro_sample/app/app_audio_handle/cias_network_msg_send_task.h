/*
 * @FileName:: 
 * @Author: 
 * @Date: 2022-04-07 10:42:30
 * @LastEditTime: 2022-04-22 17:42:09
 * @Description: 
 */
#ifndef _cias_network_msg_send_task_H_
#define _cias_network_msg_send_task_H_

#include <stdbool.h>
#include "system_msg_deal.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum
{
    SDIO_DMA_SEND = 0,
}network_task_msg_type_t;

#pragma pack(4)
typedef struct
{
   uint32_t payload;
   uint32_t length;
   uint32_t flag;
   uint8_t cmd[64];
   uint32_t type; //1发送音频数据，2交互命令 3发送音频起始包 4发送音频中间包 5发送音频结束包
}network_msg_data_t;
#pragma pack()

typedef struct
{
    network_task_msg_type_t msg_type;
    network_msg_data_t  network_data;
}sdio_task_msg_t;

void network_send_task_initial(void);
void network_send_data_task(void *p_arg);

int cais_send_cmd_to_network(unsigned short cmd);
int voice_data_packet_and_send(uint32_t packet_type,uint8_t *voice_data, int32_t packet_len);

BaseType_t send_msg_to_network_task(sdio_task_msg_t *send_msg,BaseType_t *xHigherPriorityTaskWoken);

int32_t broadlink_frame_create(uint32_t type, const uint8_t *input, uint32_t len,uint8_t *output, uint32_t maxsize,uint32_t fill_data,uint16_t index);
int32_t cias_send_cmd(uint32_t cmd, uint32_t fill_data);
int32_t cias_send_cmd_and_data(uint32_t cmd_type, uint8_t *buf, int32_t len, uint32_t fill_data);
// int cais_send_msg_flag_to_network(sDataPoint *p);
int cais_send_msg_flag_to_network(int8_t *send_buffer);
int cias_cjson_packet_and_send(uint32_t packet_type,uint8_t *data, int32_t packet_len);

#ifdef __cplusplus
}
#endif

#endif

