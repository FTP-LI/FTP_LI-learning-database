#ifndef __REMOTE_API_FOR_BNPU_H__
#define __REMOTE_API_FOR_BNPU_H__
#if CORE_ID == 1

#include "ci130x_nuclear_com.h"

extern uint32_t RC_BNPU_CALLER(get_current_model_addr)(uint32_t *p_dnn_addr, uint32_t *p_dnn_size, uint32_t *p_asr_addr, uint32_t *p_addr_size);
extern uint32_t RC_BNPU_CALLER(malloc_in_host)(size_t size);
extern void RC_BNPU_CALLER(free_in_host)(void* pv);
extern void RC_BNPU_CALLER(audio_deal_one_frm_callback)(void* para);
extern void RC_BNPU_CALLER(deal_one_frm_fft_rslt_callback)(void* para);
extern void RC_BNPU_CALLER(print_doa_angle_in_host)(int audio_state, int num);
extern void RC_BNPU_CALLER(print_pwk_db_in_host)(int db_val);
extern void RC_BNPU_CALLER(sed_rslt_cb)(void* para);
extern void RC_BNPU_CALLER(flash_get_en_data)(uint32_t scr_addr,uint8_t* data_in,uint32_t size);
extern void RC_BNPU_CALLER(alg_aes_type_tip)(int type);
extern void RC_BNPU_CALLER(alg_aes_addr)(uint8_t* data_in);
extern int RC_HOST_CALLER(get_pingyin_word)(char* split_word,int i,uint16_t* yinsu_id_list);
extern int RC_HOST_CALLER(get_data_in)(uint32_t scr_addr,float* data_in,uint32_t size);
extern int RC_HOST_CALLER(get_dqt_bias)(uint32_t scr_addr,int8_t * w, float* dqt, float* bias);
extern int RC_HOST_CALLER(set_tts_dnn_done)(uint32_t type);
extern int RC_HOST_CALLER(set_tts_init_done)(uint32_t type);
#endif
#endif

