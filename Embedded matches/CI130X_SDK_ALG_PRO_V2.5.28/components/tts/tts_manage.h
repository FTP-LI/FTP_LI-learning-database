/**
  ******************************************************************************
  * @file    audio_in_manage.h
  * @version V1.0.0
  * @date    2019.04.04
  * @brief
  ******************************************************************************
  */

#include "ringbuffer_block.h"
#include "tts_ringbuffer_block_module.h"

#ifndef __TTS_MANAGE_H_
#define __TTS_MANAGE_H_


#ifdef __cplusplus
extern "C" {
#endif

// #define TTS_MANAGE_DEBUG


typedef enum
{
    E_DNN_CACL_TYPE_ACU,
    E_DNN_CACL_TYPE_DUR,  //需要先设置 tts_dnn_set_run_dur(1)，然后判断tts_dnn_get_run_dur_status()的返回值是否是0来确定是否运算完成；
}eDnnCalcType;


typedef struct __stTTS_ringbuffer_block
{
    stbufferinfo bufferinfo;
    strbb_handle handle;
}stTTS_ringbuffer_block;

typedef struct
{
    stTTS_ringbuffer_block src_block;
    stTTS_ringbuffer_block dest_block;
}stTTS_ringbuffer_data;

typedef struct
{
    stbufferinfo src_buffer_info;
    stbufferinfo dest_buffer_info;
}stTTS_dur_data;

typedef struct __stDnn_msg
{
    eDnnCalcType dnn_type;
    union
    {
        stTTS_ringbuffer_data ring_data;
        stTTS_dur_data dur_data;
    }data;
}stDnn_msg;

typedef enum
{
    TTS_DNN_IDLE = 0,
    TTS_DNN_BUSY,
}tts_dnn_state_t;


void tts_acu_dnn_cmpt_one_time(uint32_t fe_buffer_start_addr,uint32_t rslt_addr);

void tts_dur_dnn_cmpt_one_time(uint32_t fe_buffer_start_addr,uint32_t rslt_addr);

void tts_dnn_manage_task(void*p);

int tts_manage_send_dnn_msg(stDnn_msg* pdnn_msg);

uint8_t tts_dnn_get_run_dur_status(void);

void tts_dnn_set_run_dur(uint8_t bRunDur);

void tts_dnn_xQueueReset(void);

void tts_module_init(void);
extern tts_dnn_state_t tts_dnn_state;
extern SemaphoreHandle_t dnn_done_semaphore;
void sys_tts_msg_task_initial(void);
#ifdef __cplusplus
}
#endif

#endif
