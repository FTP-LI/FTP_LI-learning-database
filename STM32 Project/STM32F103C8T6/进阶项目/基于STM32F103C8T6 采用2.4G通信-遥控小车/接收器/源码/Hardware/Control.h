#ifndef _Control_H_
#define _Control_H_

#include "OLED.h"                   //��ʾ��0.96��OLED(I2CЭ��)
#include "NRF24L01.h"               //2.4Gͨ�ţ�NRF24L01
#include "L298N.h"                  //L298N�������

void Control_Init(void);
void Control(void);


#endif
