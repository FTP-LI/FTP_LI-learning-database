#ifndef __SYSTEM_HOOK_H__
#define __SYSTEM_HOOK_H__

#include "command_info.h"


void sys_power_on_hook();

void sys_weakup_hook();

void sys_sleep_hook();

void sys_asr_result_hook(cmd_handle_t cmd_handle, uint8_t asr_score);


#endif

