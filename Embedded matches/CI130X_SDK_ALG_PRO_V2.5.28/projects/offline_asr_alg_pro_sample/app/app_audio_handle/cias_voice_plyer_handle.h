/*
 * @FileName:: 
 * @Author: 
 * @Date: 2022-04-21 13:58:52
 * @LastEditTime: 2023-05-15 13:54:37
 * @Description: 
 */
#ifndef _CIAS_VOICE_PLYER_HANDLE_H_
#define _CIAS_VOICE_PLYER_HANDLE_H_

#include "sdk_default_config.h"
#if 1
#include <stdbool.h>
#include "cias_network_msg_protocol.h"
#include "audio_play_os_port.h"

typedef enum 
{
    START_PLAY_TTS = 0,
    START_PLAY_M4A,
    START_PLAY_MP3,
}start_play_type_t;

typedef struct {
    start_play_type_t start_play;
    bool recv_media_stream_end;
    bool acc_decode_fail;
    bool mp3_decode_fail;
}media_play_resume_control;
#endif

#endif