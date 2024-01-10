#ifndef _OLED_H_
#define _OLED_H_

#include <STC15F2K60S2.H>
#include "Sys.h"

//��������

sbit OLED_SCL=P0^0;			// OLED SCLK ����
sbit OLED_SDA=P0^1;		    // OLED SDA	 ����
//���Ŷ���

#define OLED_SCL_L() OLED_SCL=0
#define OLED_SCL_H() OLED_SCL=1
 
#define OLED_SDA_L() OLED_SDA=0
#define OLED_SDA_H() OLED_SDA=1
//���Ų���

void IIC_Start();
void IIC_Stop();
void IIC_Wait_Ack();
void Write_IIC_Byte(uint8_t IIC_Byte);
void OLED_WriteCommand(uint8_t Command);
void OLED_WriteData(uint8_t Data);
void OLED_SetCursor(uint8_t Y, uint8_t X);
void OLED_Clear(void) ;
void OLED_ShowChar(uint8_t Line, uint8_t Column, char Char);                                //��ʾ�����ַ�
void OLED_ShowString(uint8_t Line, uint8_t Column, char *String);                           //��ʾ�ַ���
uint32_t OLED_Pow(uint32_t X, uint32_t Y);
void OLED_ShowNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);           //��ʾ����
void OLED_ShowSignedNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);     //��ʾ����������
void OLED_ShowHexNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);        //��ʾʮ��������
void OLED_ShowBinNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);        //��ʾ��������
void OLED_Init(void);                                                                       //��ʼ��


#endif
