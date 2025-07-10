#ifndef __SERIAL_ASR_FLOW_H
#define __SERIAL_ASR_FLOW_H


#ifdef __cplusplus
extern "C"
{
#endif


#include <stdint.h>
#include <stdbool.h>
#include "ci_log.h"
#include "ci_assert.h"
#include "asr_top_config.h"

typedef struct
{
	uint32_t fe_start_addr;
	uint32_t fe_end_addr;
	int fe_frm_num;
	uint32_t vad_end_addr;
}fe_cmpt_info_t;


typedef enum
{
	VVF_NN_NORMAL = 0xf,
	//FE不够计算一笔NN
	VVF_NN_FE_NOT_ENOUGH = 1,
	//一笔普通的NN计算
	VVF_NN_NN_HAS_RSLT = 2,
	//VAD END
	VVF_NN_VAD_END = 3,

	//打断流程分界线
	VVF_NN_CHANGE_FLOW = 0xfff,
	//有识别结果
	VVF_NN_DECODER_HAS_RESULT = 1 + 0xfff,
	//切模型
	VVF_NN_CHANGE_MODE = 2 + 0xfff,

	VVF_NN_FORCE_QUIT = 3 + 0xfff,

}vvf_nn_ret_t;


void serial_asr_flow_fe_vad_nn_init(uint32_t lut_table_addr);
int serial_asr_flow_cmpt_n_frm_nn(int nn_cmpt_parallel_num,fe_cmpt_info_t* now_p,
								fe_cmpt_info_t* fe_buffer_info,bool is_vad_start);
bool check_vad_start_fe_info_is_ok(fe_cmpt_info_t* now_p,fe_cmpt_info_t* fe_buffer_info);

vvf_nn_ret_t serial_asr_voice_vad_fe_deal(void);

#ifdef __cplusplus
}
#endif


#endif

