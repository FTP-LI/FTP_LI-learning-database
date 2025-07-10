/**
 * @file ci_denoise.h
 * @brief 
 * @version V1.0.0
 * @date 2019.07.09
 * 
 * @copyright Copyright (c) 2019 Chipintelli Technology Co., Ltd.
 * 
 */
#ifndef __VP_NN_CMPT_PORT_H
#define __VP_NN_CMPT_PORT_H


#ifdef __cplusplus
extern "C"
{
#endif

#define VP_IDENTIFY_MAX_TIME    (4)


typedef enum
{
    VP_CMPT_TYPE_NULL = 0,
    VP_CMPT_TYPE_REG,       //注册
    VP_CMPT_TYPE_IDENTIFY,  //识别
}vp_cmpt_type_t;


extern void cosin_metric(float *a, float *b,float* cos);

extern bool vp_nn_cmpt(int type,float* out_vec,int fe_len,int cmpt_time,
                        int start_frm,int valid_frm);

extern void vp_nn_cmpt_clear(void);

#ifdef __cplusplus
}
#endif


#endif //CI_DENOISE_H
