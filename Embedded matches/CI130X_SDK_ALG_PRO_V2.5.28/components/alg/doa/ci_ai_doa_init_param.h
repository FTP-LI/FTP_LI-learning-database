#ifndef __CI_AI_DOA_INIT_PARAM_H__
#define __CI_AI_DOA_INIT_PARAM_H__

#include <stdio.h>
#include <stdint.h>
typedef struct
{   
    short* feature;
    short* dense_out;
    //short**  feature_dense_buf;         //521->60
    uint8_t  vad_start_flag;            //vad 开始标记
    uint8_t  vad_end_flag;              //vad 结束标记
    uint8_t  wakeup_success_flag;       //唤醒成功标志
    short    wakeup_start_index;        //唤醒次开始帧
    short    wakeup_frame_len;          //唤醒词长度
    short    feature_dense_buf_index;      
	short*   pad_in;
	short*   Cont_m;
	short*   Cont_l;
	short*   f_tmp1;
	short*   f_tmp2;
	short*   Alpha;
	short*   Attentive_out;
	float*   logits;
    uint32_t doa_nn_model_w;
    int      doa_nn_mode;
    int      eliminateFreq;
}doa_nn_param_t;

typedef struct
{
	int denoise_mode;    
    int eliminate62_5; 
}doa_nn_config_t;
#endif   //__CI_AI_DOA_INIT_PARAM_H__