#ifndef _BEEP_VOICE_H
#define _BEEP_VOICE_H

#include "ci130x_scu.h"
#include "ci130x_gpio.h"
#include "ci130x_pwm.h"
#include "ci130x_timer.h"
#include "FreeRTOS.h"
#include "timers.h"

#if PLAY_BEEP_CMD_EN

typedef struct
{
    unsigned short tone;
    unsigned short ms;
}stBuff;

typedef struct
{
    PinPad_Name PinName;
    gpio_base_t GpioBase;
    gpio_pin_t PinNum;
    IOResue_FUNCTION PwmFun;
    IOResue_FUNCTION IoFun;
    pwm_base_t PwmBase;
}stBeepCfg;

typedef struct
{
    char bInit;
    stBeepCfg cfg;
    xTimerHandle beepTimer;
    int TimerID;
    stBuff* pBuff;
    int BeepCnt;
    int BeepIndex;
}stBeepInfo;

int BeepVoiceInit(stBeepCfg* cfg);
int BeepVoiceStart(char* buff, int len);


void cmd_beep(void);
void power_on_beep(void);
void wake_up_beep(void);
void exit_wake_up_beep(void);

void match_fail_beep(void);
void match_success_beep(void);
void exit_match_beep(void);


#endif
#endif
