#include "stdio.h"
#include "stdlib.h"
#include "ringbuffer_block.h"
#include "front_end.h"
#include "get_fe.h"
#ifndef __TTS_VOCODER_H
#define __TTS_VOCODER_H
#define VOCODER_LARGE_LEN (sizeof(float) * 5680)
#define VOCODER_BIG_LEN (sizeof(float) * 72 * 60)
#define STR_FUN_LEN 1200
#define SYLLABLES_LEN 1200
#define INPUT_NO_LEN (100 * 20)
#define ENGLISH_PLACE_LEN (100 * sizeof(EN_PLACE))
#define QU_FE_LEN (100 * sizeof(QUS_fe))
extern float *vocoder_large_buffer1;
extern int *vocoder_large_buffer2;
extern float *vocoder_big_buffer;
extern char *char_buffer_str_fun;
extern char *char_buffer_syllables;
extern char *char_buffer_input_no;
extern EN_PLACE *char_buffer_english_place;
extern QUS_fe *char_buffer_que_fe;
typedef struct
{
    stbufferinfo *acu_rbb_handl[9];
    uint16_t acu_rbb_handl_num;
    bool bFirstWorlds;
} stacu_8bufferinfo;

void tts_vocoder_task(void *p_arg);

#endif