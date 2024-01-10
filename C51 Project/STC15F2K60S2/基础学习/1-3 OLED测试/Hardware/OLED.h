#ifndef __OLED_H
#define	__OLED_H
#include "reg51.h"
#include "Sys.h"

// ------------------------------------------------------------
// IO��ģ��I2Cͨ��
// SCL��P1^0
// SDA��P1^1
// ------------------------------------------------------------
sbit OLED_SCL = P0^0; //����ʱ��
sbit OLED_SDA = P0^1; //��������
sbit OLED_RES = P1^2; //
//IIC ��ӦIO�ڸߵ͵�ƽ �궨��

#define OLED_SCL_High  OLED_SCL=1
#define OLED_SCL_Low   OLED_SCL=0

#define OLED_SDA_High  OLED_SDA=1
#define OLED_SDA_Low   OLED_SDA=0

#define OLED_RES_High  OLED_RES=1
#define OLED_RES_Low   OLED_RES=0


//��Ļ�����غ궨��
#define X_WIDTH 	128

//��Ļ�����غ궨��
#define Y_WIDTH 	64

void OLED_Init(void);
void OLED_DrawBMP(uint8_t x0,uint8_t y0,uint8_t x1,uint8_t y1,uint8_t BMP[]);//��ʾͼƬ
void OLED_ShowCN(uint8_t x, uint8_t y, uint8_t N);//��ʾ����
void OLED_ShowString(uint8_t x, uint8_t y, uint8_t ch[]);//��ʾ8*16��С���ַ�
void OLED_ShowHalfString(uint8_t x, uint8_t y, uint8_t ch[]);//��ʾ6*16��С���ַ�
void OLED_Clear(void);//����
void OLED_Fill(uint8_t fill_Data);//ȫ��Ļ���
//�û�����

void IIC_Start();
void IIC_Stop();
uint8_t Write_IIC_Byte(uint8_t IIC_Byte);
void WriteDat(uint8_t IIC_Data);
void WriteCmd(uint8_t IIC_Command);
void OLED_OFF(void);
void OLED_ON(void);
void OLED_SetPos(uint8_t x, uint8_t y);
//�û�����ģ�麯��

#endif
