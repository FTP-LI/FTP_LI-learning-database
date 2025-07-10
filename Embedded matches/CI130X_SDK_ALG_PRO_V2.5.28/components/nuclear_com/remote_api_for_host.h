
#ifndef __REMOTE_API_FOR_HOST_H__
#define __REMOTE_API_FOR_HOST_H__

#if CORE_ID == 0


#include "ci130x_nuclear_com.h"
#include "alg_preprocess.h"

extern void RC_HOST_CALLER(set_ssp_registe(audio_capture_t* audio_capture, ci_ssp_st* ci_ssp, int module_num));
extern char RC_HOST_CALLER(set_agc_gain_enable(char agc_gain_enable));
extern void RC_HOST_CALLER(set_freqvad_start_para_gain(short vad_start_gain_t));
extern void RC_HOST_CALLER(set_doa_out_type(int doa_out_type));
extern void RC_HOST_CALLER(vad_rollback_frm_cfg(short vad_back_frm));
extern void RC_HOST_CALLER(req_cmpt_vp)(int type,float* mould_buf,bool* is_ok,float* cos,int cmpt_time,
                                        int start_frm,int valid_frm);
extern void RC_HOST_CALLER(vp_nn_cmpt_cfg)(int len,int skip_num);
extern void RC_HOST_CALLER(sed_nn_cmpt_cfg)(int len,int skip_num);
extern void RC_HOST_CALLER(vp_cmpt_clear)(void);
extern void RC_HOST_CALLER(set_eq_gain_str(void* p));
extern void RC_HOST_CALLER(set_drc_config_str(void* p));
extern int  RC_HOST_CALLER(get_aes_state)();
extern void RC_HOST_CALLER(req_tts_cmpt)(int type,float* dest_addr,float* res_addr);
extern void RC_HOST_CALLER(req_tts_mgc2sp)(float* mgc, float* sp_fe);
#endif
#endif

