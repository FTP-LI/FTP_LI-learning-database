#ifndef _MIDDLE_DEVICE_H
#define _MIDDLE_DEVICE_H

#include "command_info.h"
#include "system_msg_deal.h"

void userapp_deal_asr_msg_ex(sys_msg_asr_data_t *asr_msg);//语音控制红外功能
void userapp_deal_com_msg_ex(sys_msg_com_data_t *com_msg);//串口控制红外功能
void userapp_deal_com_msg_semantic_id(int semantic_id);
#endif
