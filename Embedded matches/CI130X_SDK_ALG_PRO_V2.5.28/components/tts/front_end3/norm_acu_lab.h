//Macro_Definition
#include <stdio.h>
#include <stdint.h>
#include "macro_definition.h"
#include "ci_log.h"
#include "ci_tts.h"


/*
typedef struct
{
    uint8_t frame_split_part;
    uint8_t frame_split_num;
    uint8_t dur_id;
    uint8_t dur_f;
    float *buffer_excit_end;
    float *buffer_excit_front;
    float *buffer_Y;
    float buffer_f0;
}Buffer_split;
*/
void InitializeBuffer_split( Buffer_split* buffer);
void DestroyBuffer_split(Buffer_split* buffer);


int morm_acu_lab_calc_one_index(uint16_t* acu_lab, uint16_t fdata, int index);
void norm_acu_lab_split(uint16_t* lab, uint16_t dur_num, uint16_t* acu_lab, float* acu_out ,  uint16_t* dur, Buffer_split* buffer_split );