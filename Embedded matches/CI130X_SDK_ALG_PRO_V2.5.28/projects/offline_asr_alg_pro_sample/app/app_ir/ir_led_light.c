#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "system_msg_deal.h"
#include "prompt_player.h"
#include "voice_module_uart_protocol.h"
#include "i2c_protocol_module.h"
#include "ci_nvdata_manage.h"
#include "ci_log.h"
#include "ci130x_gpio.h"
#include "ir_led_light.h"

#include "common_api.h"
#include "buzzer_voice.h"
#include "ci130x_dpmu.h"

///tag-insert-code-pos-1
xTimerHandle xTimerDevice = NULL;
QueueHandle_t device_queue = NULL;

#define LED_FLASH_SUPPORT

#ifdef LED_FLASH_SUPPORT
xTimerHandle xTimerLedFlash = NULL;
static unsigned int uiLedFlashCnt = 0;
#endif

void user_led_light_on(void)
{
    #ifdef LED_FLASH_SUPPORT
    //低电平有效
    gpio_set_output_level_single(PA, pin_5,1);
    #endif
}

void user_led_light_off(void)
{
    #ifdef LED_FLASH_SUPPORT
     gpio_set_output_level_single(PA, pin_5,0);
    #endif
}

#ifdef LED_FLASH_SUPPORT
void LedFlashTimerCallback(TimerHandle_t xTimer)
{
    if (0 == uiLedFlashCnt)
    {
        if(xTimerLedFlash != NULL)
            xTimerStop(xTimerLedFlash,0);
        if (SYS_STATE_WAKEUP == get_wakeup_state())
        {
            user_led_light_on();
        }
        else
        {
            user_led_light_off();
        }
        goto out;
    }

    if (uiLedFlashCnt%2 == 1)
    {
        user_led_light_on();
    }
    else
    {
        user_led_light_off();
    }

    uiLedFlashCnt--;

out:
    return;
}
#endif

void user_led_light_init(void)
{
    #ifdef LED_FLASH_SUPPORT
    scu_set_device_gate(PA,ENABLE);
    dpmu_set_io_direction(PA5,DPMU_IO_DIRECTION_OUTPUT);
    dpmu_set_io_reuse(PA5,FIRST_FUNCTION);/*gpio function*/
    gpio_set_output_mode(PA, pin_5);
    gpio_set_output_level_single(PA, pin_5,0);

    xTimerLedFlash = xTimerCreate("xTimerLedFlash", (pdMS_TO_TICKS(100)), pdTRUE, (void *)0, LedFlashTimerCallback);
    if(!xTimerLedFlash)
    {
        mprintf("xTimerLedFlash fail!\n");
    }
    #endif
}


void user_led_light_flash(void)
{
     #ifdef LED_FLASH_SUPPORT
     uiLedFlashCnt = 6;
     if(xTimerLedFlash != NULL)
        xTimerStart(xTimerLedFlash,0);
     #endif
}

int ir_task_init()
{
    //红外需要创建任务队列和timer
    device_queue = xQueueCreate(10, sizeof(device_msg));
    if(!device_queue)
    {
        mprintf("device_queue fail\n");
    }
 //创建timer心跳，用于给设备做定时任务。心跳周期是DEVICE_TIME，不需要很精确的任务

    ContinueSendKey_list_init();
    xTimerDevice = xTimerCreate("xTimerDevice", (pdMS_TO_TICKS(DEVICE_TIME)), pdTRUE, (void *)0, DeviceTimerCallback);
    if(!xTimerDevice)
    {
        mprintf("xTimerMain fail!\n");
    }
    /* 系统监控任务 */
#if (1 == PLAY_BEEP_CMD_EN)
    stBeepCfg beepconfig;

    beepconfig.PinName    = PWM0_PAD;
    beepconfig.PinNum     = gpio_pin_1;
    beepconfig.PwmBase    = PWM0;

    beepconfig.GpioBase   = GPIO1;
    beepconfig.PwmFun     = SECOND_FUNCTION;
    beepconfig.IoFun      = FIRST_FUNCTION;

    BeepVoiceInit(&beepconfig);

    power_on_beep();

#endif
    user_led_light_init();

}