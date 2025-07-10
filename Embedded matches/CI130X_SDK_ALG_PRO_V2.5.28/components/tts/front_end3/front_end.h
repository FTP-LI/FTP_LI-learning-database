#ifndef __FORNT_DEND_H
#define __FORNT_DEND_H
#include "ci_log.h"
typedef struct
{
	uint16_t location;
	char phone[10];
}PY_MARK;

typedef struct
{
	int english;
	int place;
}EN_PLACE;

typedef struct
{
    int utt_sy;
    int utt_word;
    int utt_phrase;   
}UTT_FE;

typedef struct
{
    int phrase_sy;
    int phrase_word;
    int word_sy;
}PHRASE_FE;

typedef struct
{
    int sy_word[2];
    int sy_phrase[2];
    int sy_utt[2];
    int word_phrase[2];
    int word_utt[2];
    int phrase_utt[2];
}CQS_FE;

#endif
