/**
 * @file system_msg_deal.c
 * @brief  系统消息处理任务
 * @version V1.0.0
 * @date 2019.01.22
 *
 * @copyright Copyright (c) 2019  Chipintelli Technology Co., Ltd.
 *
 */
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"
#include "ci_log.h"
#include "audio_play_api.h"
#include "ci130x_uart.h"
#include "sdk_default_config.h"
#include "ci130x_iisdma.h"
#include "ci130x_iis.h"
#include "ci130x_lowpower.h"
#include "ci130x_core_misc.h"
#include "ci_adapt_aec.h"
#include "prompt_player.h"
#include "product_semantic.h"
#include "ci_nvdata_manage.h"
#include "asr_api.h"
#include "status_share.h"
#include "codec_manager.h"
#include "ci_nlp_control.h"
#include "system_msg_deal.h"
#include "nlp_msg_deal.h"



static QueueHandle_t nlp_msg_queue = NULL;
/**
 * @brief 系统消息任务资源初始化
 *
 */
void nlp_msg_initial(void)
{
    nlp_msg_queue = xQueueCreate(8, sizeof(nlp_msg_t));
    if(!nlp_msg_queue)
    {
        mprintf("not enough memory:%d,%s\n",__LINE__,__FUNCTION__);
    }
}

/**
 * @brief A simple code for other component send system message to this module, just wrap freertos queue function
 *
 * @param flag_from_isr : 0 call this function not from isr, other call this from isr
 * @param send_msg : system message
 * @param xHigherPriorityTaskWoken : if call this not from isr set NULL
 * @return BaseType_t
 */
BaseType_t send_msg_to_nlp_task(nlp_msg_t *send_msg,BaseType_t *xHigherPriorityTaskWoken)
{
    if(0 != check_curr_trap())
    {
        return xQueueSendFromISR(nlp_msg_queue,send_msg,xHigherPriorityTaskWoken);
    }
    else
    {
        return xQueueSend(nlp_msg_queue,send_msg,0);
    }
}

extern int asr_result_callback_nlp(char *cmd, int cfd, int decoded_frames, short sli_cfd, short path_node_cfd, short start_frame, short frame_len);

void nlpTaskManageProcess(void *p_arg)
{
	nlp_msg_t rev_msg;
	BaseType_t err = pdPASS;  
	nlp_timer_init();
	while(1)
	{
		/* 阻塞接收系统消息 */
		err = xQueueReceive(nlp_msg_queue, &rev_msg, portMAX_DELAY);

		if(pdPASS == err)
		{
			/* 根据消息来源来处理对应消息，用户可以自己创建属于自己的系统消息类型 */
			switch (rev_msg.msg_type)
			{
				case NLP_MSG_TYPE_NLP:
				{
					if(rev_msg.msg_data.nlp_data.flag == 1)
					{
						asr_result_callback_nlp("。", 3221, 1, 0, 0, 0, 0);
					}
					else if(rev_msg.msg_data.nlp_data.flag == 2)
					{
						asr_result_callback_nlp("。", 3222, 1, 0, 0, 0, 0);
					}
					else if(rev_msg.msg_data.nlp_data.flag == 3)
					{
						asr_result_callback_nlp("。", 3223, 1, 0, 0, 0, 0);
					}

					break;
				}
				case NLP_MSG_TYPE_NLP_RESULT:
				{
					uint8_t i;
					sys_msg_t send_msg;
					send_msg.msg_type = SYS_MSG_TYPE_ASR;
					send_msg.msg_data.asr_data.asr_status = MSG_ASR_STATUS_GOOD_RESULT;
					for(i=0; i<rev_msg.msg_data.nlp_data.intent_count;i++)
					{
						cmd_handle_t cmd_handle = cmd_info_find_command_by_string(rev_msg.msg_data.nlp_data.intent[i]);
						send_msg.msg_data.asr_data.asr_cmd_handle = cmd_handle;
						send_msg_to_sys_task(&send_msg, NULL);
					}
					break;
				}
				default:
					break;
			}
		}
	}
}
