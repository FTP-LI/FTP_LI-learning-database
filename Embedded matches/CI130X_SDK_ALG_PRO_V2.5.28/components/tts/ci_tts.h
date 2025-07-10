/**
  ******************************************************************************
  * @file    ci_tts.h
  * @version V1.0.0
  * @date    2022.02.14
  * @brief 
  ******************************************************************************
  */

#ifndef __CI_TTS_H_
#define __CI_TTS_H_
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

  // void tts_manage_task(void* p);

  typedef struct
  {
    uint8_t frame_split_part;
    uint8_t frame_split_num;
    uint8_t dur_id;
    uint8_t dur_f;
    float *buffer_excit_end;
    float *buffer_excit_front;
    float *buffer_Y;
    float *buffer_Y_front;
    float buffer_f0;
    float *mgc_60;
    float *lf0;
    float *bap;
  } Buffer_split;

#ifdef __cplusplus
}
#endif

#endif
