#ifndef __OLED_H
#define	__OLED_H
#include "reg51.h"
#include "Sys.h"

// ------------------------------------------------------------
// IO口模拟I2C通信
// SCL接P1^0
// SDA接P1^1
// ------------------------------------------------------------
sbit OLED_SCL = P0^0; //串行时钟
sbit OLED_SDA = P0^1; //串行数据
sbit OLED_RES = P1^2; //
//IIC 对应IO口高低电平 宏定义

#define OLED_SCL_High  OLED_SCL=1
#define OLED_SCL_Low   OLED_SCL=0

#define OLED_SDA_High  OLED_SDA=1
#define OLED_SDA_Low   OLED_SDA=0

#define OLED_RES_High  OLED_RES=1
#define OLED_RES_Low   OLED_RES=0


//屏幕长像素宏定义
#define X_WIDTH 	128

//屏幕高像素宏定义
#define Y_WIDTH 	64

void OLED_Init(void);
void OLED_DrawBMP(uint8_t x0,uint8_t y0,uint8_t x1,uint8_t y1,uint8_t BMP[]);//显示图片
void OLED_ShowCN(uint8_t x, uint8_t y, uint8_t N);//显示中文
void OLED_ShowString(uint8_t x, uint8_t y, uint8_t ch[]);//显示8*16大小的字符
void OLED_ShowHalfString(uint8_t x, uint8_t y, uint8_t ch[]);//显示6*16大小的字符
void OLED_Clear(void);//清屏
void OLED_Fill(uint8_t fill_Data);//全屏幕填充
//用户函数

void IIC_Start();
void IIC_Stop();
uint8_t Write_IIC_Byte(uint8_t IIC_Byte);
void WriteDat(uint8_t IIC_Data);
void WriteCmd(uint8_t IIC_Command);
void OLED_OFF(void);
void OLED_ON(void);
void OLED_SetPos(uint8_t x, uint8_t y);
//用户函数模块函数

#endif
