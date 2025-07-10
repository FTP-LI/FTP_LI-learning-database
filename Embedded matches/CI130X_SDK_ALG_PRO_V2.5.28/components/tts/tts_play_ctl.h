#include "stdio.h"
#include "stdlib.h"

#ifndef __TTS_PLAY_CTL_H
#define __TTS_PLAY_CTL_H

// 0 - 9
int tts_play_ctl_set_vol(int vol);

// 0 - 9
int tts_play_ctl_set_speed(int vol);

// 0 - 9
int tts_play_ctl_set_intonation(int vol);


// 开始播放
int tts_play_ctl_start();

int tts_play_ctl_stop();

int tts_play_ctl_pause();

int tts_play_ctl_resume();
int tts_play_ctl_status();

extern bool STOP_FALG;
extern bool STOP_FRINT_END_FALG;
extern bool STOP_ACU_DUR_FALG;
extern bool STOP_VOCODER_FALG;
extern bool STOP_AUDIO_FALG;
#endif