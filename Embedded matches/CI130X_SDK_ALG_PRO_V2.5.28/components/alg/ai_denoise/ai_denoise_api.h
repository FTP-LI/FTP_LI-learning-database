#ifndef AI_DENOISE_API_H
#define AI_DENOISE_API_H 


#include <stdio.h>
#include <stdbool.h>
#ifdef __cplusplus
extern "C"{
#endif

typedef struct
{
	bool alg_enable;
	float alpha_forget;
	int denoise_mode;    

}denoise_nn_config_t;

int ci_ai_denoise_version( void );
void* ci_ai_denoise_create(void* module_config);

int ci_ai_denoise_deal(void* handle, float* fft_in, float* fft_out);

void ci_ai_denoise_npu_init(void);

#ifdef __cplusplus
}
#endif
#endif