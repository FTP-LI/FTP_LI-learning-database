/**
  ******************************************************************************
  * @file    ci110x_speex.c
  * @version V1.0.0
  * @date    2018.07.31
  * @brief   speex编码器统一接口
  ******************************************************************************
  */

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "speex.h"
#include "speex_bits.h"
#include "speex_types.h"
#include "speex_header.h"
#include "speex_stereo.h"
#include "speex_config_types.h"
#include "ci110x_speex.h"
#include "FreeRTOS.h"

#if 1

// extern void *pvPortCalloc( size_t nmemb, size_t size );
extern void *pvPortMalloc(size_t xWantedSize);
// extern void *pvPortRealloc( void *pv, size_t size );
extern void vPortFree(void *pv);

#define speex_exit exit
#define malloc pvPortMalloc
// #define calloc  pvPortCalloc
// #define realloc pvPortRealloc
#define free vPortFree

#endif

unsigned char *g_speex_out_p = NULL;
SpeexMode *cias_speex_wb_mode = NULL;
//unsigned char g_speex_result[100*1024];

void *ci_speex_create(void)
{

    ci_speex_t *speex = NULL;
    int32_t quality = 5;
    // int32_t complexity = 4;
    int32_t tmp = 0;
     //ci_loginfo(LOG_VOICE_UPLOAD, "0-Heap left: %dKB\n", get_heap_bytes_remaining_size() / 1024);
    speex = (ci_speex_t *)malloc(sizeof(ci_speex_t));
    if (speex == NULL)
    {
        mprintf("Memory overflow!!! Alloc speex handler failed.\n");
    }
    // speex->_frame_cur = 0;
    memset(speex, 0, sizeof(ci_speex_t));
    //ci_loginfo(LOG_VOICE_UPLOAD, "1-Heap left: %dKB\n", get_heap_bytes_remaining_size() / 1024);
    speex_bits_init(&speex->_bits);
    //ci_loginfo(LOG_VOICE_UPLOAD, "2-Heap left: %dKB\n", get_heap_bytes_remaining_size() / 1024);  //cias_speex_wb_mode
    speex->_encoder = speex_encoder_init(cias_speex_wb_mode); /*16K采样率*/  //speex_nb_mode-20ms一帧 160个采样点 speex_wb_mode-320个采样点
    //ci_loginfo(LOG_VOICE_UPLOAD, "3-Heap left: %dKB\n", get_heap_bytes_remaining_size() / 1024);
    if (speex->_encoder == NULL)
    {
        mprintf("Speex encoder state init failed!\n");
    }

    
    //ci_loginfo(LOG_VOICE_UPLOAD, "4-Heap left: %dKB\n", get_heap_bytes_remaining_size() / 1024);
    speex_encoder_ctl(speex->_encoder, SPEEX_SET_QUALITY, &quality);
    //ci_loginfo(LOG_VOICE_UPLOAD, "5-Heap left: %dKB\n", get_heap_bytes_remaining_size() / 1024);
    speex_encoder_ctl(speex->_encoder, SPEEX_GET_FRAME_SIZE, &speex->_frame_size);
    //ci_loginfo(LOG_VOICE_UPLOAD, "6-Heap left: %dKB\n", get_heap_bytes_remaining_size() / 1024);

    speex->_buffer_out = (char *)malloc(speex->_frame_size*2);
    if (speex->_buffer_out == NULL)
    {
        mprintf("Memory overflow!!! Alloc speex buffer out failed.\n");
    }
    //ci_loginfo(LOG_VOICE_UPLOAD, "7-Heap left: %dKB\n", get_heap_bytes_remaining_size() / 1024);

    speex->_buffer_in = (short *)malloc(sizeof(short) * (speex->_frame_size));   //malloc大小必须和实际数据大小符合，不然会出现解码后开头数据错误
    if (speex->_buffer_in == NULL)
    {
        mprintf("Memory overflow!!! Alloc speex buffer in failed.\n");
    }
    //ci_loginfo(LOG_VOICE_UPLOAD, "8-Heap left: %dKB\n", get_heap_bytes_remaining_size() / 1024);

    speex->_buffer_in_float = (float *)malloc(sizeof(float) * (speex->_frame_size));   //malloc大小必须和实际数据大小符合，不然会出现解码后开头数据错误
    if (speex->_buffer_in_float == NULL)
    {
        mprintf("Memory overflow!!! Alloc speex buffer in failed.\n");
    }
    //ci_loginfo(LOG_VOICE_UPLOAD, "9-Heap left: %dKB\n", get_heap_bytes_remaining_size() / 1024);

    mprintf("ci_speex_create,Speex frame size: %d\n", speex->_frame_size);


    // ci_speex_destroy(speex);

    // ci_loginfo(LOG_VOICE_UPLOAD, "10-Heap left: %dKB\n", get_heap_bytes_remaining_size() / 1024);
    // while(1)
    // {
    //     ci_loginfo(LOG_VOICE_UPLOAD, "11-Heap left: %dKB\n", get_heap_bytes_remaining_size() / 1024);
    //     vTaskDelay(100);
    // }

    return speex;
}

void ci_speex_destroy(void *handler)
{
    ci_speex_t *speex = (ci_speex_t *)handler;
    mprintf("ci_speex_destroy start\r\n");

    if (speex)
    {
        if (speex->_encoder)
        {
            speex_encoder_destroy(speex->_encoder);
            speex->_encoder = NULL;
        }

        if (speex->_buffer_out)
        {
            free(speex->_buffer_out);
            speex->_buffer_out = NULL;
        }

        if (speex->_buffer_in)
        {
            free(speex->_buffer_in);
            speex->_buffer_in = NULL;
        }
        if (speex->_buffer_in_float)
        {
            free(speex->_buffer_in_float);
            speex->_buffer_in_float = NULL;
        }
        speex_bits_destroy(&speex->_bits);
        free(speex);
        speex = NULL;
    }
    
    mprintf("ci_speex_destroy end\r\n");
}

int32_t cias_speex_compressed_data(ci_speex_t *_speex, int8_t *data, char *raw_addr, unsigned int size)
{
    int32_t written = 0;
    int32_t ret = 0;
    ci_speex_t *speex = (ci_speex_t *)_speex;
    unsigned int cur = 0;
    if (speex && speex->_encoder && speex->_buffer_in && speex->_buffer_out)
    {
        // mprintf("speex_encode1: size = %d, framesize = %d\n", size, speex->_frame_size);
        #if 1
        
        for (int i = 0; i < speex->_frame_size; i++)
        {
            speex->_buffer_in[i] = ((uint8_t)data[2 * i] | ((uint8_t)data[2 * i + 1] << 8)); 
            speex->_buffer_in_float[i] = speex->_buffer_in[i];
        }
        speex_bits_reset(&speex->_bits);
        speex_encode(speex->_encoder, speex->_buffer_in_float, &speex->_bits);
        written = speex_bits_write(&speex->_bits, speex->_buffer_out+1, speex->_frame_size);
        speex->_buffer_out[0] = (char)written;
        written++;

        // mprintf("-----mprintf = %d\r\n", written);
        memcpy(raw_addr, speex->_buffer_out, written); 
        //raw_addr += written;
        return written;
        #endif
#if 0
        if(CI_SPEEX_INT == speex->ci_speex_mode)
        {
            //mprintf("CI_SPEEX_INT  !!!\n");
            size_t framesize = speex->_frame_size;
            short *buffer_in = speex->_buffer_in;

            while (cur + framesize - speex->_frame_cur <= size)
            {
                mprintf("duer_speex_encode1: cur = %d, speex->_frame_cur = %d, framesize - speex->_frame_cur = %d\n", cur, speex->_frame_cur, framesize - speex->_frame_cur);
                memcpy(buffer_in + speex->_frame_cur, data + cur, (framesize - speex->_frame_cur));
                cur += framesize - speex->_frame_cur;
                speex->_frame_cur = 0;

                speex_bits_reset(&speex->_bits);

               
                /*Encode the frame*/
                for (int i=0;i<framesize/2;i++)
                    speex->_buffer_in_float[i] = speex->_buffer_in[i];

                speex_encode(speex->_encoder, speex->_buffer_in_float, &speex->_bits);
              

                written = speex_bits_write(&speex->_bits, speex->_buffer_out+2, speex->_frame_size);
                speex->_buffer_out[0] = (char)written;
                speex->_buffer_out[1] = (char)(written + 1);
                written++;
                written++;
                mprintf("Riven Debug 1(%d)\n",written);

                memcpy(raw_addr,speex->_buffer_out,written);    
               // raw_addr += written;
                ret += written;
            }


            if (cur < size)
            {
                memcpy(buffer_in + speex->_frame_cur, data + cur, (size - cur)*sizeof(short));
                speex->_frame_cur += size - cur;

                mprintf("duer_speex_encode2: cur = %d, speex->_frame_cur = %d\n", cur, speex->_frame_cur);
                if (speex->_frame_cur > 0)
                {
                    memset(buffer_in + speex->_frame_cur, 0, (framesize - speex->_frame_cur)*sizeof(short));
                    speex_bits_reset(&speex->_bits);
                    
                    
                    for (int i=0;i<speex->_frame_cur/2;i++)
                        speex->_buffer_in_float[i] = speex->_buffer_in[i];

                    speex_encode(speex->_encoder, speex->_buffer_in_float, &speex->_bits);
                    

                    written = speex_bits_write(&speex->_bits, speex->_buffer_out+1, speex->_frame_size);
                    speex->_buffer_out[0] = (char)written;
                    written ++;
                  //  mprintf("Riven Debug 2(%d)\n",written);

                    memcpy(raw_addr,speex->_buffer_out,written);
                 //   raw_addr += written;
                    ret += written;
                }
            }


        }
        else if(CI_SPEEX_FLOAT == speex->ci_speex_mode)
        {
            mprintf("CI_SPEEX_FLOAT  !!!\n");
            size_t framesize = speex->_frame_size;
            float *buffer_in = speex->_buffer_in_float;

            mprintf("speex_encode2: size = %d, framesize = %d\n", size, framesize);

            float tem[320];
            while (cur + framesize - speex->_frame_cur <= size)
            {
                for(unsigned int i = 0; i < 320;i++)
                {
                    tem[i] = (float)(data[i+cur]);
                }

                mprintf("duer_speex_encode: cur = %d, speex->_frame_cur = %d\n", cur, speex->_frame_cur);
                memcpy(buffer_in + speex->_frame_cur, tem, (framesize - speex->_frame_cur)*sizeof(float));
                cur += framesize - speex->_frame_cur;
                speex->_frame_cur = 0;
                speex_bits_reset(&speex->_bits);
                /*Encode the frame*/
                speex_encode(speex->_encoder, speex->_buffer_in_float, &speex->_bits);
                written = speex_bits_write(&speex->_bits, speex->_buffer_out, speex->_frame_size);
                mprintf("written = %d\n",written);
                memcpy(raw_addr,speex->_buffer_out,written);
                raw_addr += written;
            }


            if (cur < size)
            {
                for(unsigned int i = 0; i < 320;i++)
                {
                    tem[i] = (float)(data[i+cur]);
                }

                memcpy(buffer_in + speex->_frame_cur, tem, (size - cur)*sizeof(float));
                speex->_frame_cur += size - cur;

                mprintf("duer_speex_encode xxx: cur = %d, speex->_frame_cur = %d\n", cur, speex->_frame_cur);

                if (speex->_frame_cur > 0)
                {
                    memset(buffer_in + speex->_frame_cur, 0, (framesize - speex->_frame_cur)*sizeof(float));
                    speex_bits_reset(&speex->_bits);
                    speex_encode(speex->_encoder, speex->_buffer_in_float, &speex->_bits);
                    written = speex_bits_write(&speex->_bits, speex->_buffer_out, speex->_frame_size);
                    mprintf("written = %d\n",written);
                    memcpy(g_speex_out_p,speex->_buffer_out,written);
                    g_speex_out_p += written;
                }
            }
        }
#endif
    }

    return ret;
}

#if AUDIO_COMPRESS_SPEEX_ENABLE
void speex_test(unsigned int pcm_addr, unsigned int raw_addr, unsigned int pcm_size)
{
    ci_speex_t *ci_speex_hander;
    ci_speex_hander = ci_speex_create();
    if (NULL == ci_speex_hander)
    {
        return;
    }
    ci_speex_hander->ci_speex_mode = CI_SPEEX_INT;

    cias_speex_compressed_data(ci_speex_hander, (short *)pcm_addr, (unsigned char *)raw_addr, pcm_size / sizeof(short));

    ci_speex_destroy(ci_speex_hander);
}
#endif

/***************************************************************************************************/

// static SpeexBits ebits;
// static void *enc_state = NULL;
// static bool g_codc_open = false;

// int tvs_speex_encode(char* pcm, int pcm_size, char* codec_buffer, int codec_buffer_size) {
// 	if (!g_codc_open) {
// 		return 0;
// 	}
// 	int nbBits = 0;

//     mprintf("Riven Debug 0\n");
// 	speex_bits_reset(&ebits);

//     mprintf("Riven Debug 1\n");
// 	speex_encode_int(enc_state, (short*)pcm, &ebits);

//     mprintf("Riven Debug 2\n");
// 	nbBits = speex_bits_write(&ebits, codec_buffer + 1, codec_buffer_size);
// 	codec_buffer[0] = (char)nbBits;
// 	nbBits++;

//     mprintf("Riven Debug 3\n");
// 	return nbBits;
// }

// int tvs_speex_enc_open(int compress) {
// 	if (g_codc_open) {
// 		return 0;
// 	}

// 	g_codc_open = true;
// 	speex_bits_init(&ebits);
// 	enc_state = speex_encoder_init(&speex_wb_mode);
// 	speex_encoder_ctl(enc_state, SPEEX_SET_QUALITY, &compress);
// 	int32_t framesize = 0;
// 	speex_encoder_ctl(enc_state, SPEEX_GET_FRAME_SIZE, &framesize);
// 	mprintf("speex encoder open, framesize %d, compress %d\n", framesize, compress);
// 	return 0;
// }

// void tvs_speex_enc_close() {
// 	if (!g_codc_open) {
// 		return;
// 	}
// 	g_codc_open = false;
// 	speex_bits_destroy(&ebits);
// 	speex_encoder_destroy(enc_state);
// 	mprintf("speex encoder close\n");
// }
