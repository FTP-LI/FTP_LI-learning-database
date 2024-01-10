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

void IIC_Wait_Ack()//����һ��ʱ�ӣ�������Ӧ���ź�
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
    Write_IIC_Byte(0x78);//��ַ
    IIC_Wait_Ack();
    Write_IIC_Byte(0x00);//ָ��
    IIC_Wait_Ack();    
    Write_IIC_Byte(Command); 
    IIC_Wait_Ack();
    IIC_Stop();
}

void OLED_WriteData(uint8_t Data)
{
    IIC_Start();
    Write_IIC_Byte(0x78);//��ַ
    IIC_Wait_Ack();    
    Write_IIC_Byte(0x40);//����			
    IIC_Wait_Ack();
    Write_IIC_Byte(Data);
    IIC_Wait_Ack();
    IIC_Stop();
}

void OLED_SetCursor(uint8_t Y, uint8_t X)//OLED���ù��λ�� 
{ 	
    OLED_WriteCommand(0xB0 | Y);					//����Yλ��
	OLED_WriteCommand(0x10 | ((X & 0xF0) >> 4));	//����Xλ�ø�4λ
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

void OLED_ShowChar(uint8_t Line, uint8_t Column, char Char)//��ʾһ���ַ�
{      	
	uint8_t i;
	OLED_SetCursor((Line - 1) * 2, (Column - 1) * 8);		//���ù��λ�����ϰ벿��
	for (i = 0; i < 8; i++)
	{
		OLED_WriteData(OLED_F8x16[Char - ' '][i]);			//��ʾ�ϰ벿������
	}
	OLED_SetCursor((Line - 1) * 2 + 1, (Column - 1) * 8);	//���ù��λ�����°벿��
	for (i = 0; i < 8; i++)
	{
		OLED_WriteData(OLED_F8x16[Char - ' '][i + 8]);		//��ʾ�°벿������
	}
}

void OLED_ShowString(uint8_t Line, uint8_t Column, char *String)//��ʾ�ַ���
{
	uint8_t i;
	for (i = 0; String[i] != '\0'; i++)
	{
		OLED_ShowChar(Line, Column + i, String[i]);
	}
}

uint32_t OLED_Pow(uint32_t X, uint32_t Y)//OLED�η�����
{
	uint32_t Result = 1;
	while (Y--)
	{
		Result *= X;
	}
	return Result;
}


void OLED_ShowNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)//OLED��ʾ���֣�ʮ���ƣ�������
{
	uint8_t i;
	for (i = 0; i < Length; i++)							
	{
		OLED_ShowChar(Line, Column + i, Number / OLED_Pow(10, Length - i - 1) % 10 + '0');
	}
}

void OLED_ShowSignedNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)//OLED��ʾ���֣�ʮ���ƣ�����������
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

void OLED_ShowHexNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)//OLED��ʾ���֣�ʮ�����ƣ�������
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

void OLED_ShowBinNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)//OLED��ʾ���֣������ƣ�������
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
	for (i = 0; i < 1000; i++)			//�ϵ���ʱ
	{
		for (j = 0; j < 1000; j++);
	}
	OLED_WriteCommand(0xAE);	//�ر���ʾ
    OLED_WriteCommand(0x00);
    OLED_WriteCommand(0x10);
    OLED_WriteCommand(0x40);	//������ʾ��ʼ��
    OLED_WriteCommand(0xB0);
    OLED_WriteCommand(0x81);	//���öԱȶȿ���
    OLED_WriteCommand(0xFF);	
    OLED_WriteCommand(0xA1);	//�������ҷ���0xA1���� 0xA0���ҷ���
    OLED_WriteCommand(0xA6);
    OLED_WriteCommand(0xA8);
    OLED_WriteCommand(0x3F);
    OLED_WriteCommand(0xC8);	//�������·���0xC8���� 0xC0���·���
    OLED_WriteCommand(0xD3);
    OLED_WriteCommand(0x00);
    
    OLED_WriteCommand(0xD5);
    OLED_WriteCommand(0x80);
    
    OLED_WriteCommand(0xD8);
    OLED_WriteCommand(0x05);
    
	OLED_WriteCommand(0xD9);	//����Ԥ�������
	OLED_WriteCommand(0xF1);

	OLED_WriteCommand(0xDA);	//����������ʾ��/�ر�
    OLED_WriteCommand(0x12);
    
    OLED_WriteCommand(0xDB);    //����COM����Ӳ������
    OLED_WriteCommand(0x30);

	OLED_WriteCommand(0x8D);	//���ó���
	OLED_WriteCommand(0x14);

	OLED_WriteCommand(0xAF);	//������ʾ
		
	OLED_Clear();				//OLED����
}  
 