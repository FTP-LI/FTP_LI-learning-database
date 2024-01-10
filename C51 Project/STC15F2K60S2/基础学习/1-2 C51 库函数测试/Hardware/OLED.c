#include <REG51.H>
#include "OLED.h"
#include "OLED_Font.h"
		
void IIC_Start()
{
	OLED_SCL_H() ;
	OLED_SDA_H();
	OLED_SDA_L();
	OLED_SDA_L();
}
 
void IIC_Stop()
{
    OLED_SDA_L();
    OLED_SCL_H() ;
	OLED_SDA_H();
}

void IIC_Wait_Ack()//额外一个时钟，不处理应答信号
{
	OLED_SCL_H() ;
	OLED_SCL_L();
}

void Write_IIC_Byte(uint8_t IIC_Byte)
{
	unsigned char i;
	unsigned char send,adata;
	adata=IIC_Byte;
	OLED_SCL_L();
	for(i=0;i<8;i++)		
	{
        send=adata;
		send=send&0x80;
		if(send==0x80)
		{
            OLED_SDA_H();
        }
		else
        {            
            OLED_SDA_L();
        }
		adata=adata<<1;
		OLED_SCL_H();
		OLED_SCL_L();
	}
}

void OLED_WriteCommand(uint8_t Command)
{
    IIC_Start();
    Write_IIC_Byte(0x78);//地址
    IIC_Wait_Ack();
    Write_IIC_Byte(0x00);//指令
    IIC_Wait_Ack();    
    Write_IIC_Byte(Command); 
    IIC_Wait_Ack();
    IIC_Stop();
}

void OLED_WriteData(uint8_t Data)
{
    IIC_Start();
    Write_IIC_Byte(0x78);//地址
    IIC_Wait_Ack();    
    Write_IIC_Byte(0x40);//数据			
    IIC_Wait_Ack();
    Write_IIC_Byte(Data);
    IIC_Wait_Ack();
    IIC_Stop();
}

void OLED_SetCursor(uint8_t Y, uint8_t X)//OLED设置光标位置 
{ 	
    OLED_WriteCommand(0xB0 | Y);					//设置Y位置
	OLED_WriteCommand(0x10 | ((X & 0xF0) >> 4));	//设置X位置高4位
	OLED_WriteCommand(0x00 | (X & 0x0F));	
}   	  
	   
void OLED_Clear(void)  
{  
	uint8_t i, j;
	for (j = 0; j < 8; j++)
	{
		OLED_SetCursor(j, 0);
		for(i = 0; i < 128; i++)
		{
			OLED_WriteData(0x00);
		}
	}
}

void OLED_ShowChar(uint8_t Line, uint8_t Column, char Char)//显示一个字符
{      	
	uint8_t i;
	OLED_SetCursor((Line - 1) * 2, (Column - 1) * 8);		//设置光标位置在上半部分
	for (i = 0; i < 8; i++)
	{
		OLED_WriteData(OLED_F8x16[Char - ' '][i]);			//显示上半部分内容
	}
	OLED_SetCursor((Line - 1) * 2 + 1, (Column - 1) * 8);	//设置光标位置在下半部分
	for (i = 0; i < 8; i++)
	{
		OLED_WriteData(OLED_F8x16[Char - ' '][i + 8]);		//显示下半部分内容
	}
}

void OLED_ShowString(uint8_t Line, uint8_t Column, char *String)//显示字符串
{
	uint8_t i;
	for (i = 0; String[i] != '\0'; i++)
	{
		OLED_ShowChar(Line, Column + i, String[i]);
	}
}

uint32_t OLED_Pow(uint32_t X, uint32_t Y)//OLED次方函数
{
	uint32_t Result = 1;
	while (Y--)
	{
		Result *= X;
	}
	return Result;
}


void OLED_ShowNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)//OLED显示数字（十进制，正数）
{
	uint8_t i;
	for (i = 0; i < Length; i++)							
	{
		OLED_ShowChar(Line, Column + i, Number / OLED_Pow(10, Length - i - 1) % 10 + '0');
	}
}

void OLED_ShowSignedNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)//OLED显示数字（十进制，带符号数）
{
	uint8_t i;
	uint32_t Number1;
	if (Number >= 0)
	{
		OLED_ShowChar(Line, Column, '+');
		Number1 = Number;
	}
	else
	{
		OLED_ShowChar(Line, Column, '-');
		Number1 = -Number;
	}
	for (i = 0; i < Length; i++)							
	{
		OLED_ShowChar(Line, Column + i + 1, Number1 / OLED_Pow(10, Length - i - 1) % 10 + '0');
	}
}

void OLED_ShowHexNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)//OLED显示数字（十六进制，正数）
{
	uint8_t i, SingleNumber;
	for (i = 0; i < Length; i++)							
	{
		SingleNumber = Number / OLED_Pow(16, Length - i - 1) % 16;
		if (SingleNumber < 10)
		{
			OLED_ShowChar(Line, Column + i, SingleNumber + '0');
		}
		else
		{
			OLED_ShowChar(Line, Column + i, SingleNumber - 10 + 'A');
		}
	}
}

void OLED_ShowBinNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)//OLED显示数字（二进制，正数）
{
	uint8_t i;
	for (i = 0; i < Length; i++)							
	{
		OLED_ShowChar(Line, Column + i, Number / OLED_Pow(2, Length - i - 1) % 2 + '0');
	}
}
 
void OLED_Init(void)
{ 	
    uint32_t i, j;
	for (i = 0; i < 1000; i++)			//上电延时
	{
		for (j = 0; j < 1000; j++);
	}
	OLED_WriteCommand(0xAE);	//关闭显示
    OLED_WriteCommand(0x00);
    OLED_WriteCommand(0x10);
    OLED_WriteCommand(0x40);	//设置显示开始行
    OLED_WriteCommand(0xB0);
    OLED_WriteCommand(0x81);	//设置对比度控制
    OLED_WriteCommand(0xFF);	
    OLED_WriteCommand(0xA1);	//设置左右方向，0xA1正常 0xA0左右反置
    OLED_WriteCommand(0xA6);
    OLED_WriteCommand(0xA8);
    OLED_WriteCommand(0x3F);
    OLED_WriteCommand(0xC8);	//设置上下方向，0xC8正常 0xC0上下反置
    OLED_WriteCommand(0xD3);
    OLED_WriteCommand(0x00);
    
    OLED_WriteCommand(0xD5);
    OLED_WriteCommand(0x80);
    
    OLED_WriteCommand(0xD8);
    OLED_WriteCommand(0x05);
    
	OLED_WriteCommand(0xD9);	//设置预充电周期
	OLED_WriteCommand(0xF1);

	OLED_WriteCommand(0xDA);	//设置整个显示打开/关闭
    OLED_WriteCommand(0x12);
    
    OLED_WriteCommand(0xDB);    //设置COM引脚硬件配置
    OLED_WriteCommand(0x30);

	OLED_WriteCommand(0x8D);	//设置充电泵
	OLED_WriteCommand(0x14);

	OLED_WriteCommand(0xAF);	//开启显示
		
	OLED_Clear();				//OLED清屏
}  
 