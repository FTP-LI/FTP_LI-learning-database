#ifndef _TTS_GB2313TOUTF8_H
#define _TTS_GB2313TOUTF8_H
#include "stdint.h"
int GetUtf8ByteNumForWord(unsigned char firstCh);
unsigned short int SearchCodeTable_flash(unsigned short int gbKey);
int enc_unicode_to_utf8_one(unsigned long unic, unsigned char *pOutput, int outSize);
int Gb2312ToUtf8(unsigned char *gb, int len, unsigned char *utfArray);
void tts_code_deal_Gb2312ToUtf8(unsigned char *gb_buffer, unsigned char len, unsigned char *utfArray);
extern void get_dictionary_file_addr();
#define FLASH_CPU_READ_BASE_ADDR (0x50000000U)
#define GB2312TOUNICODE_TABLE_SIZE (6768 + 69)
///字典
#pragma pack(8)
typedef struct
{
    uint32_t gb2312toutf8_file_addr;
    uint32_t gb2312toutf8_file_size;
    uint32_t one_word_file_addr;
    uint32_t one_word_file_size;
    uint32_t two_word_file_addr;
    uint32_t two_word_file_size;
    uint32_t three_word_file_addr;
    uint32_t three_word_file_size;
    uint32_t four_word_file_addr;
    uint32_t four_word_file_size;

//提示音
    uint32_t ring1_file_addr;
    uint32_t ring1_file_size;
    uint32_t ring2_file_addr;
    uint32_t ring2_file_size;
    uint32_t ring3_file_addr;
    uint32_t ring3_file_size;
    uint32_t ring4_file_addr;
    uint32_t ring4_file_size;
    uint32_t ring5_file_addr;
    uint32_t ring5_file_size;

    uint32_t message1_file_addr;
    uint32_t message1_file_size;
    uint32_t message2_file_addr;
    uint32_t message2_file_size;
    uint32_t message3_file_addr;
    uint32_t message3_file_size;
    uint32_t message4_file_addr;
    uint32_t message4_file_size;
    uint32_t message5_file_addr;
    uint32_t message5_file_size;

    uint32_t alert1_file_addr;
    uint32_t alert1_file_size;
    uint32_t alert2_file_addr;
    uint32_t alert2_file_size;
    uint32_t alert3_file_addr;
    uint32_t alert3_file_size;
    uint32_t alert4_file_addr;
    uint32_t alert4_file_size;
    uint32_t alert5_file_addr;
    uint32_t alert5_file_size;

    uint32_t mel_addr;   // const float mel_to_linear_matrix_sparse[497][3]
    uint32_t mel_size;
    uint32_t noise_addr;  //const float noise_seed[1024][3]
    uint32_t noise_size;
    uint32_t pulse_addr;   //const float pulse_seed[512][3]
    uint32_t pulse_size;
    uint32_t py2phone_addr;  //const struct DICT_phone py2phone[419]
    uint32_t py2phone_size;

        
} dictionary_file_t;
typedef struct
{
	unsigned char word[14];
	unsigned short int y_id1;
	unsigned short int y_id2;
	unsigned short int y_id3;
	unsigned short int y_id4;
}DICT_world_4;

typedef struct
{
	unsigned char word[10];
	unsigned short int y_id1;
	unsigned short int y_id2;
	unsigned short int y_id3;
}DICT_world_3;

typedef struct
{
	unsigned char word[8];
	unsigned short int y_id1;
	unsigned short int y_id2;
}DICT_world_2;

typedef struct
{
	unsigned char word[4];
	unsigned short int y_id1;
}DICT_world_1;
extern dictionary_file_t *get_dictionary_struct_addr();
#endif