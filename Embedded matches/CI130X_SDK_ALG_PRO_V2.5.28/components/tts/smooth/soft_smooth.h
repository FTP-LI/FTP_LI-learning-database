#include <stdlib.h>
#include <stdio.h>
#include "ci_tts.h"
typedef struct
{
    int dim;
    int len;
    int capacity;
    float *data;
} FrameSeq;

FrameSeq *FrameSeq_new(int dim, int capacity);
void FrameSeq_del(FrameSeq *fs);
void FrameSeq_pushframe(FrameSeq *fs, const float *frame_data);
FrameSeq *FrameSeq_transcopy(const FrameSeq *fs);

FrameSeq *mlpgGO(FrameSeq *features, FrameSeq *covariance, const int static_dimension);
void postfilter_mlpg(float *mlpg_out, float *lf0_out, float *ap_out, Buffer_split *buffer);