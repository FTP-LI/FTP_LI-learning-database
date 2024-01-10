#ifndef __PWM_H__
#define __PWM_H__

#include "Sys.h"
#include <STC15F2K60S2.H>

 
void PWM_Init(void);
void Set_CCP2Pwm(uint8_t pwm);
void Set_CCP2_2Pwm(uint8_t pwm);
void Set_CCP1_2Pwm(uint8_t pwm);
void Set_CCP2_2Pwm(uint8_t pwm);


#endif
