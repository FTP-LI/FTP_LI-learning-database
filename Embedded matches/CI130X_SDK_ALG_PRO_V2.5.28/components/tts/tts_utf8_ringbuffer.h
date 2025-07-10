#include "stdio.h"
#include "stdlib.h"
#include "ringbuffer.h"

#ifndef __TTS_UTF8_RINGBUFFER_H
#define __TTS_UTF8_RINGBUFFER_H

ringbuffer_t* tts_utf8_get_rb_handle(void);
ringbuffer_t* tts_ring_get_rb_handle(void);
#endif