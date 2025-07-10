#include "ringbuffer.h"
#include "system_msg_deal.h"
#include "voice_module_uart_tts_protocol.h"
#ifndef _TTS_SERIAL_TEXT_DEAL_H
#define _TTS_SERIAL_TEXT_DEAL_H

#define GB2312BufferSIZE    (6+PACKET_MIN_SIZE)
#define utfArraySIZE        (((PACKET_MIN_SIZE+2)/2)*3)

void tts_seial_reveice_task(void *p_arg);
uint32_t deal_serial_msg(sys_tts_msg_t *msg);
int tts_receive_rb_init(uint32_t buffer_size);
ringbuffer_t* tts_receive_get_rb_handle(void);
extern volatile QueueHandle_t sys_msg_queue;


#endif