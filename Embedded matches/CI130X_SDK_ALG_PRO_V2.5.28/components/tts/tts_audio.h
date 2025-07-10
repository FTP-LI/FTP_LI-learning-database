#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "stdio.h"
#include "stdlib.h"
#include "ringbuffer_block.h"

#ifndef __TTS_AUDIO_H
#define __TTS_AUDIO_H


void tts_player_varialbes_init(void);
void tts_audio_player_task(void *p_arg);
int tts_audio_get_stream_len(void);
int32_t tts_audio_player_deal(int16_t*msgbuf , int32_t msglen);
void tts_audio_player_deal_pcm(int16_t *buffer, int32_t msglen);
BaseType_t tts_audio_xStreamBufferReset(void);
int adjustmentVolume(int16_t *buffer, int numSamples);
#endif