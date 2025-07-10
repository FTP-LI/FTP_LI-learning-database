#include "ringbuffer_block.h"


#ifndef __TTS_RINGBUFFER_BLOCK_MODE_H
#define __TTS_RINGBUFFER_BLOCK_MODE_H

typedef unsigned int strbb_handle;

typedef struct __stTTs_all_ringbuffer_block
{
    strbb_handle font_end_buffer;
    strbb_handle befor_acu_buffer;
    strbb_handle acu_buffer;
    strbb_handle dur_buffer;
    strbb_handle audio_buffer;
}stTTs_all_ringbuffer_block;

strbb_handle tts_get_font_end_rbb(void);

strbb_handle tts_get_befor_acu_rbb(void);
strbb_handle tts_get_dur_rbb(void);
strbb_handle tts_get_acu_rbb(void);

strbb_handle tts_get_audio_rbb(void);

int tts_rbb_module_init(void);

int tts_rbb_module_clear(void);

int tts_rbb_module_free(void);

uint32_t tts_rbb_module_available(strbb_handle rbb_handle);

uint32_t tts_rbb_module_size(strbb_handle rbb_handle);

// 计算malloc和push的块数
uint32_t tts_rbb_module_malloc_push_size(strbb_handle rbb_handle);

uint32_t tts_rbb_module_getpullsize(strbb_handle rbb_handle);

// 除去free的空间，其余所有的
uint32_t tts_rbb_module_used_size(strbb_handle rbb_handle);

void tts_rbb_module_freeblock(strbb_handle rbb_handle);

void tts_rbb_module_pushblock(strbb_handle rbb_handle);

void tts_rbb_module_print_info(void);

stbufferinfo* tts_rbb_module_get_pushblock(strbb_handle rbb_handle);

stbufferinfo* tts_rbb_module_get_freeblock(strbb_handle rbb_handle);



#endif