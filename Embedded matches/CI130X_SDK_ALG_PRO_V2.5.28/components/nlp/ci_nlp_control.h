// 输出nlp_delayd时间

/*WAIT_NLP_NEXT_TIME:下一个意图识别，等待最长时间设置，设置范围0~7档，默认设置4档1680ms；
*1、最长等待时间，与长词条识别关联高，长词条在第二和第三个意图中出现时，
*当上一个意图识别到后，需要考虑说出下一个长词条的间隔时间，加上说出来长
*词条所需的时间；
*2、两个时间相加会比较长。对于七八个字以上的长词，可能至少需要等待1800毫秒
*以上，才能较好的识别长词条的意图。
*这个等待时间，起始值为960ms，步进180ms，每个等级时间如下:
*0:960ms, 1:1140ms, 2:1320ms, 3:1500ms
*4:1680ms, 5:1860ms, 6:2040ms, 7:2220ms
*/
#ifndef WAIT_NLP_NEXT_TIME
#define WAIT_NLP_NEXT_TIME               4
#endif

/*NLP_NEXT_DELAY_TIME:下一个意图识别，有效解码输出的超时等待时间，设置范围0~7档，默认设置3档900ms；
*此超时时间说明分以下三种情况：
*1、等待下一个意图识别的过程中，如果持续产生有效的解码输出。不是第三个意图时，
*会重置这个超时时间。会有以下两种情况发生：
*A、在WAIT_NLP_NEXT_TIME超时时间内，出现有效意图识别，那么当前意图识别流程结束，
*继续下一个意图识别流程；
*B、在WAIT_NLP_NEXT_TIME超时时间内，没能正确解码有效意图，整个多意图识别流程结束，
*输出最终多意图识别结果；
*2、等待下一个意图识别的过程中，如果持续产生有效的解码输出。并且，此时是第三个意
*图识别过程中，不会重置这个超时时间。会在这个超时时间内，解码第三个意图，最终可
*能输出，也可能不输出第三个意图，并且结束多意图解码流程。可以看出，有三意图的情况
*下，最后第三个意图超时时间是NLP_NEXT_DELAY_TIME，而不是WAIT_NLP_NEXT_TIME；
*3、等待下一个意图识别的过程中，如果持续无有效的解码输出。那么，当这个超时
*等待时间结束后，会快速结束多意图识别流程。通常，在安静环境中，结束多意图
*识别流程，就是这种超时结束情况生效。
*这个等待时间，起始值为720ms，步进60ms，每个等级时间如下:
*0:720ms, 1:780ms, 2:840ms, 3:900ms
*4:960ms, 5:1020ms, 6:1080ms, 7:1140ms
*/
#ifndef NLP_NEXT_DELAY_TIME
#define NLP_NEXT_DELAY_TIME              3
#endif

void nlp_timer_init(void);
void set_state_nlp_end(void);
void update_nlp_next_time(int wait_time);
int get_wait_time();


void nlp_delay_init(void);
void update_nlp_delay_time(int wait_time);
void set_nlp_delay_end(void);
int get_delay_time();