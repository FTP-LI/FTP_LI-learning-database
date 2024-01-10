#ifndef __PWM_H__
#define __PWM_H__

#include "Sys.h"
#include <STC15F2K60S2.H>

void PWM0_Init(void);
void Timer0_Init(void);
void Steer_SetAngle(uint8_t Angle);

#endif
