/**
  ******************************************************************************
  * @file    audio_in_manage.h
  * @version V1.0.0
  * @date    2019.04.04
  * @brief
  ******************************************************************************
  */

#ifndef __TTS_CONV_H_
#define __TTS_CONV_H_
#include "NN_memmap.h"

#ifdef __cplusplus
extern "C" {
#endif
#define LAYER0_BN_LEN 1024
typedef struct
{
    uint32_t bn_len;
    uint32_t bn_addr;
    float *bn_para_addr;
}dnn_bn_tts_config_t;

void tts_conv_init(void);
void tts_acu_conv_cmpt_one_time(uint32_t src_addr,uint32_t rslt_addr);
void tts_dur_conv_cmpt_one_time(uint32_t src_addr,uint32_t rslt_addr);

#ifdef __cplusplus
}
#endif

#endif
