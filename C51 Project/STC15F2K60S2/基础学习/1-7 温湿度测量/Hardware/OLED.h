#ifndef _OLED_H_
#define _OLED_H_

#include <STC15F2K60S2.H>
#include "Sys.h"

//变量声明

sbit OLED_SCL=P0^0;			// OLED SCLK 引脚
sbit OLED_SDA=P0^1;		    // OLED SDA	 引脚
//引脚定义

#define OLED_SCL_L() OLED_SCL=0
#define OLED_SCL_H() OLED_SCL=1
 
#define OLED_SDA_L() OLED_SDA=0
#define OLED_SDA_H() OLED_SDA=1
//引脚操作

void IIC_Start();
void IIC_Stop();
void IIC_Wait_Ack();
void Write_IIC_Byte(uint8_t IIC_Byte);
void OLED_WriteCommand(uint8_t Command);
void OLED_WriteData(uint8_t Data);
void OLED_SetCursor(uint8_t Y, uint8_t X);
void OLED_Clear(void) ;
void OLED_ShowChar(uint8_t Line, uint8_t Column, char Char);                                //显示单个字符
void OLED_ShowString(uint8_t Line, uint8_t Column, char *String);                           //显示字符串
uint32_t OLED_Pow(uint32_t X, uint32_t Y);
void OLED_ShowNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);           //显示数字
void OLED_ShowSignedNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);     //显示带符号数字
void OLED_ShowHexNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);        //显示十六进制数
void OLED_ShowBinNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);        //显示二进制数
void OLED_Init(void);                                                                       //初始化


#endif
