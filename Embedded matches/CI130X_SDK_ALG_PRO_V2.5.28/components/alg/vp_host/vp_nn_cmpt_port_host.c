#include <stdint.h>
#include <stdbool.h>
#include "ci_log.h"
#include "FreeRTOS.h"
#include "task.h"
#include <string.h>
#include "ci_assert.h"
#include "vp_nn_cmpt_port_host.h"
#include "status_share.h"
#include "remote_api_for_host.h"


static inline float hard_fsqrt(float x)
{
	float y;
	asm volatile("fsqrt.s %0,%1":"=f"(y):"f"(x));
	return y;
}

void cosin_metric(float *a, float *b,float* cos)
{
	float m=0.0f;
	float n_a=0.0f;
	float n_b=0.0f;
	float res = 0.0f;
	for(int i=0;i<192;i++)
	{
		m = m+a[i]*b[i];
		n_a = n_a + a[i]* a[i];
		n_b = n_b + b[i]* b[i];
	}
	n_a = hard_fsqrt(n_a);
	n_b = hard_fsqrt(n_b);
	res = m / (n_a * n_b);
	*cos = res;
}


bool vp_nn_cmpt(int type,float* out_vec,int fe_len,int cmpt_time,
				int start_frm,int valid_frm)
{
	bool is_ok = true;
	float cos;
	REMOTE_CALL(req_cmpt_vp(type, out_vec, &is_ok,&cos,cmpt_time,start_frm,valid_frm));

	return is_ok;
}


void vp_nn_cmpt_clear(void)
{
	REMOTE_CALL(vp_cmpt_clear());
}


