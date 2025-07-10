/**
 * @file system_msg_deal.c
 * @brief  
 * @version V1.0.0
 * @date 2019.01.22
 * 
 * @copyright Copyright (c) 2019  Chipintelli Technology Co., Ltd.
 * 
 */
#ifndef _SYSTEM_NLP_MSG_DEAL_H_
#define _SYSTEM_NLP_MSG_DEAL_H_

#include "FreeRTOS.h"
#include "task.h"
#include "sdk_default_config.h"
#include "user_config.h"
#include "command_info.h"
#include "ci_nlp.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum
{
    NLP_MSG_TYPE_START = 0,
    NLP_MSG_TYPE_NLP,
    NLP_MSG_TYPE_NLP_RESULT,
}nlp_msg_type_t;


typedef struct
{
	nlp_msg_type_t msg_type;/*here will be modify use union*/
	union
	{
		nlp_data_t nlp_data;
	}msg_data;
}nlp_msg_t;

BaseType_t send_msg_to_nlp_task(nlp_msg_t *send_msg, BaseType_t *xHigherPriorityTaskWoken);
void nlp_msg_initial(void);
void nlpTaskManageProcess(void *p_arg);
#ifdef __cplusplus
}
#endif
  
#endif


