#ifndef __CI_PWK_H__
#define __CI_PWK_H__

#include <stdio.h>
#include "ci_audio_wrapfft.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct
{   
    uint8_t  vad_start_flag;            //vad 开始标记
    uint8_t  vad_end_flag;              //vad 结束标记    
}pwk_param_t;

typedef struct 
{
    bool alg_enable;
}pwk_config_t;

pwk_param_t *get_ci_pwk_param(void);
int ci_pwk_processing(void *handle, void *wrapfft_audio_t);
void* ci_pwk_module_init(void *module_config);

#ifdef __cplusplus
}
#endif

#endif    //__CI_PWK_H__