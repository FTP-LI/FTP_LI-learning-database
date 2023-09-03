#ifndef _Control_H_
#define _Control_H_

#include "OLED.h"                   //��ʾ��0.96��OLED(I2CЭ��)
#include "NRF24L01.h"               //2.4Gͨ�ţ�NRF24L01
#include "AD.h"                     //ҡ�����ݣ�HW-504
#include "MPU6050.h"                //��п��ƣ�MPU6050
#include "key.h"                    //������������
#include "Delay.h"

void Control_Init(void);
uint8_t Yao_Gan_Control(void);
uint8_t Ti_Gan_Control(void);
uint8_t Control(uint8_t mod,uint8_t come);
void OLED_Base(void);
void OLED_Base_Y(void);
void OLED_Base_T(void);

#endif
