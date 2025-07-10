/**
  ******************************************************************************
  * @file    ci110x_speex.h
  * @version V1.0.0
  * @date    2018.07.31
  * @brief   speex编码器统一接口
  ******************************************************************************
  */
#ifndef _CI110X_SPEEX_H
#define _CI110X_SPEEX_H

#include "speex.h"
#include "speex_bits.h"
#include "speex_types.h"

#if 0
#if (defined(__CC_ARM) || defined(__ICCARM__))
#include "ci_log.h"
#else
#define mprintf printf
#endif
#endif


typedef enum
{
    CI_SPEEX_INT = 0,
    CI_SPEEX_FLOAT = 1,
}CI_SPEEX_MODE;


typedef struct _ci_speex_s
{
    void *  _encoder;

    unsigned int    _frame_size;
    unsigned int    _frame_cur;
    //union
    //{
        short * _buffer_in;
        float * _buffer_in_float;
    //};

    char *  _buffer_out;

    SpeexBits   _bits;
    CI_SPEEX_MODE ci_speex_mode;
} ci_speex_t;

void *ci_speex_create(void);
void ci_speex_destroy(void * handler);
// int32_t ci_voice_send(ci_speex_t *_speex,int8_t* data, char* raw_addr,unsigned int size);
void speex_test(unsigned int pcm_addr,unsigned int raw_addr,unsigned int pcm_size);







/***********************************************/
int tvs_speex_encode(char* pcm, int pcm_size, char* codec_buffer, int codec_buffer_size);
int tvs_speex_enc_open(int compress);

void tvs_speex_enc_close() ;
#endif /*_CI110X_SPEEX_H*/
