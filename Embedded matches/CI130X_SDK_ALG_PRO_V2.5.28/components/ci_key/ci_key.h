#ifndef __CI_KEY_H
#define __CI_KEY_H

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "timers.h"

#include <string.h>
#include "ci130x_system.h"
#include "ci130x_core_eclic.h"
#include "ci130x_scu.h"
#include "ci130x_dpmu.h"
#include "ci130x_adc.h"
#include "system_msg_deal.h"


#include "system_msg_deal.h"

#ifdef __cplusplus
extern "C"
{
#endif

//!按键消抖时间为2*10ms
#define KEY_SHORTPRESS_TIMER    (2)
//!按键长按间隔时间为150*10ms
#define KEY_LONGPRESS_TIMER     (100)

/**
 * @addtogroup key
 * @{
 */
void ci_key_gpio_init(void);
void ci_key_io_init(void);
void ci_key_init(void);
void key_info_show(sys_msg_key_data_t *key_msg);
void HEMY_Timer_Create();
void AutoReloadCallback( TimerHandle_t xTimer );
void HEMY_Timer_Create_START(void);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif


#endif

