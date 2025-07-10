/**
  ******************************************************************************
  * @file    audio_in_manage.h
  * @version V1.0.0
  * @date    2019.04.04
  * @brief
  ******************************************************************************
  */

#ifndef __TTS_DNN_H_
#define __TTS_DNN_H_
#include "NN_memmap.h"

#ifdef __cplusplus
extern "C" {
#endif
#define SCALES_INT       (1024.0f)
#define DQT_FAETURE      (0.0009765625f)//(1.0f/SCALES_INT)    
#define DIV_SCALE        (3.0518509475997192297128208258309e-5f)//(1.0f/32767.0f)
#define MAX_VAL          (31.999f)
#define MIN_VAL          (-32.0f)

#define MAX_VAL_INT16    (32767.0f)
#define MIN_VAL_INT16    (-32768.0f)

#define SRAM_LOAD_START_ADDR  (0x1ffd0000)
#define FLASH_LOAD_START_ADDR (0x50000000)
#define TTS_MODE_HEAD        2048

#define LOAD_W_A_START_ADDR W_A_BUFFER_ADDR
#define LOAD_W_B_START_ADDR W_B_BUFFER_ADDR


#define INPUT_SCALE 28 //分段大小
#define INPUT_SCALE_CNT 20 //w参数分段个数

#define INPUT_W_ONECE 16 //一次加载到W buffer的参数行数
#define INPUT_W_TIMES 32 //加载到W buffer的次数
#define TTS_NN_CMPT_IN_BUFFER_SIZE   (8*280) //参数个数
#define TTS_NN_OUT_BUFFER_SIZE   (8*560) //参数个数

typedef struct
{
    uint16_t dim_toal_len;   //神经元个数
    uint8_t dim_len;        //每次warp运算的结果维度，即每次加载神经元的个数
    uint8_t pack_len;       //每行W，X参数的分段大小，
    uint8_t scale_cnt;       //分段个数
    uint8_t x_input;        //x参数输入维度大小（列数）
    float x_dqt[2];         //X参数的dqt，一般float转int16是乘1024，所以dtq为DQT_FAETURE
    uint32_t mode_addr;
}dnn_tts_config_t;

typedef struct 
{
	unsigned int addr;
	unsigned int size;
	unsigned int dimsize;
	unsigned int segnum;
}tts_model_info_t;

extern  short * tts_tcm_in_buffer_addr; //nn运算的x 在tcm中地址
extern float * tts_tcm_out_buffer_addr; //nn运算结果在tcm中地址
extern short * data_in_short_addr;  //输入样本x转定点后地址
extern float * data_out_float_addr;  //输出x样本地址
extern short * data_res_addr;

void tts_dnn_init(void);
void tts_linear(dnn_tts_config_t dnn_tts, short *in_short_addr, float *out_float_addr);
void tts_res_linear(dnn_tts_config_t dnn_tts, short *in_short_addr, float *out_float_addr, int res_block_number);
void tts_acu_dnn_cmpt_one_time(uint32_t src_addr,uint32_t rslt_addr);
void tts_dur_dnn_cmpt_one_time(uint32_t src_addr,uint32_t rslt_addr);
void get_dur_acu_in(uint32_t src_addr);
#ifdef __cplusplus
}
#endif

#endif
