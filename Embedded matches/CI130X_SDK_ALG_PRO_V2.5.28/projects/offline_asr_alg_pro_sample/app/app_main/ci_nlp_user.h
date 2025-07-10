#ifndef __CI_NLP_USER_H__
#define __CI_NLP_USER_H__

#include "asr_process_callback_decoder.h"
#include "ci_nlp.h"

extern int nlp_module_init(void);
void nlp_result(nlp_data_t res, short is_wake_words, int cmd_words_cfd[4]);
void nlp_intents_sort(nlp_data_t *res);
#endif