#ifndef _CIAS_VOICE_UPLOAD_H_
#define _CIAS_VOICE_UPLOAD_H_
#include "cias_audio_data_handle.h"

#if VOICE_UPLOAD_BY_UART
void voice_upload_enable(void);
void voice_upload_disable(void);
int voice_upload_task_init(void);
#endif

#endif //_CIAS_VOICE_UPLOAD_H_