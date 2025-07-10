#include "stdio.h"
#include "stdlib.h"
#include "ringbuffer_block.h"
#include "tts_vocoder.h"
#include "ci_tts.h"

#ifndef __BACK_END_H
#define __BACK_END_H

void mlpg_for_buffer(stacu_8bufferinfo* pacu_8buffer, float *mlpg_out, float *lf0_out,  float *ap_out);
void postfilter(float* mgc, int framenumber);
int vocoder(float *Y, float *mlpg_out, float *lf0_out, float *ap_out, Buffer_split* buffer);

#endif
