#include "OLED.h"
#include "OLED_Font.h"
#include "Delay.h"

//��ʱ������IIC��ACKӦ��λ
void IIC_Delay(void)
{
	uint8_t D_t=1;
	while(D_t--);
}

//IIC Start
void IIC_Start()
{
   OLED_SCL_High;		
   OLED_SDA_High;
	 IIC_Delay();
   OLED_SDA_Low;
	 IIC_Delay();
   OLED_SCL_Low;
}

//IIC Stop
void IIC_Stop()
{
   OLED_SCL_Low;
   OLED_SDA_Low;
	 IIC_Delay();
	 OLED_SCL_High;		
   OLED_SDA_High;
}

//�ȴ��ź���Ӧ
uint8_t I2C_WaitAck(void) //�������źŵĵ�ƽ
{
	uint8_t Ack_Sta;
	OLED_SDA_High;
	IIC_Delay();
	OLED_SCL_High;
	IIC_Delay();
	if(OLED_SDA)
	{
		Ack_Sta = 1;
	}
	else
	{
		Ack_Sta = 0;
	}
	OLED_SCL_Low;
	IIC_Delay();
	return Ack_Sta;
}

// ͨ��I2C����дһ���ֽ�
uint8_t Write_IIC_Byte(uint8_t IIC_Byte)
{
	uint8_t i;
	for(i=0;i<8;i++)
	{
		if(IIC_Byte & 0x80)
		{
			OLED_SDA_High;
		}
		else
		{
			OLED_SDA_Low;
		}
		IIC_Delay();
		OLED_SCL_High;
		IIC_Delay();
		OLED_SCL_Low;
		IIC_Byte<<=1;
	}
  return I2C_WaitAck();
}

//OLEDд����
void WriteDat(uint8_t IIC_Data)
{
	IIC_Start();
	Write_IIC_Byte(0x78);
	Write_IIC_Byte(0x40);			//write data
	Write_IIC_Byte(IIC_Data);
	IIC_Stop();
}

//OLEDд����
void WriteCmd(uint8_t IIC_Command)
{
	IIC_Start();
	Write_IIC_Byte(0x78);            //Slave address,SA0=0
	Write_IIC_Byte(0x00);			//write command
	Write_IIC_Byte(IIC_Command);
	IIC_Stop();
}

void OLED_Init(void)//��ʼ��
{
	OLED_RES_Low;
	delay_ms(200);//�ϵ��ӳ�
	OLED_RES_High;
    
	WriteCmd(0xAE); //�ر���ʾ
	
	WriteCmd(0x20);	//Set Memory Addressing Mode  //�����ڴ�ˮƽѰַ��ʽ	
	WriteCmd(0x10);	//00,ˮƽ��Ѱַģʽ;01,��ֱ��Ѱַģʽ;10,ҳѰַ (RESET);11,��Ч��
	
	WriteCmd(0xB0);	//Set Page Start Address for Page Addressing Mode,0-7  //��ҳ��Ѱַ��ʽ��ѡ��ҳ����ʼ��ַ��
	
	WriteCmd(0xC8);	//Set COM Output Scan Direction C0 //����ģʽ(RESET)ɨ���COM0��COM[N -1] C8 ����ģʽ(RESET)ɨ���COM[N -1]��COM0
	
	WriteCmd(0x00); //---set low column address//����ҳ��Ѱַģʽ��������ʼ��λ��ַ
	WriteCmd(0x10); //---set high column address//����ҳ��Ѱַģʽ��������ʼ��λλ��ַ
	
	WriteCmd(0x40); //--set start line address //������ʾRAM��ʾ��ʼ�мĴ���
	
	WriteCmd(0x81); //--set contrast control register//���öԱȶȣ�Ҳ��������
	WriteCmd(0xCF); //���ȵ��� 0x00~0xff
	
	WriteCmd(0xA1); //--set segment re-map 0 to 127 //A0 �е�ַ0ӳ�䵽SEG0 A1�е�ַ127ӳ��ΪSEG0
	
	WriteCmd(0xA6); //--set normal display  //A6 0��1�� A7 1��0��
	
	WriteCmd(0xA8); //--set multiplex ratio(1 to 64) //MUX�������ΪN+1 MUX
	WriteCmd(0x3F); //1/64duty  //����Ҫˢ��COM�� 16-64��Ч
	
	WriteCmd(0xA4); //0xa4,Output follows RAM content;0xa5,Output ignores RAM content //A4 ���RAM���� A5 ����RAM����
	
	WriteCmd(0xD3); //-set display offset //���ô�ֱ�ƶ���COM��0d~63dreset���ֵ������Ϊ00h
	WriteCmd(0x00); //-not offset
	
	WriteCmd(0xD5); //--set display clock divide ratio/oscillator frequency//������ʾʱ�ӷ�Ƶ/����Ƶ��
	WriteCmd(0xF0); //--set divide ratio
	
	WriteCmd(0xD9); //--set pre-charge period//����pre-charge����
	WriteCmd(0x22); //  ��������
	
	WriteCmd(0xDA); //--set com pins hardware configuration  ����COM����Ӳ������ 
	WriteCmd(0x12);  //����COM�������� ����COM��/����ӳ��
	
	WriteCmd(0xDB); //--set vcomh //����VCOMHȡ����ѡ������
	WriteCmd(0x20); //0x20,0.77xVcc
	
	WriteCmd(0x8D); //--set DC-DC enable//����ֱ����Դ
	WriteCmd(0x14); //
	
	WriteCmd(0xAF); //--turn on oled panel  //����Ļ��ʾ
	OLED_Fill(0x00); //��ʼ����
} 


//OLED �������
void OLED_SetPos(uint8_t x, uint8_t y) //������ʼ������
{ 
	WriteCmd(0xb0+y);//��ҳ��Ѱַ��ʽ��ѡ��ҳ����ʼ��ַ��
	WriteCmd(((x&0xf0)>>4)|0x10);  //����ҳ��Ѱַģʽ��������ʼ��λ��ַ
	WriteCmd((x&0x0f)|0x01);   //����ҳ��Ѱַģʽ��������ʼ��λ��ַ
}

//OLEDȫ�����
void OLED_Fill(uint8_t fill_Data)
{
	uint8_t m,n;
	for(m=0;m<8;m++)
	{
		WriteCmd(0xb0+m);
		WriteCmd(0x00);		
		WriteCmd(0x10);		
		for(n=0;n<X_WIDTH;n++) 
			{
				WriteDat(fill_Data);
				
			}
	}
}
//OLED����
void OLED_Clear(void)
{
	OLED_Fill(0x00);
}

//��OLED�������л���

void OLED_ON(void)
{
	WriteCmd(0X8D);  //���õ�ɱ�
	WriteCmd(0X14);  //������ɱ�
	WriteCmd(0XAF);  //OLED����
}

//��OLED����
void OLED_OFF(void)
{
	WriteCmd(0X8D);  //���õ�ɱ�
	WriteCmd(0X10);  //�رյ�ɱ�
	WriteCmd(0XAE);  //OLED����
}

void OLED_ShowString(uint8_t x, uint8_t y, uint8_t ch[])//(x:0~127, y:0~7)
{
	uint8_t c = 0,i = 0,j = 0;
	while(ch[j] != '\0')
        {
			c = ch[j] - 32;
			if(x > 120)
			{
				x = 0;
				y++;
			}
			OLED_SetPos(x,y);
			for(i=0;i<8;i++)
				WriteDat(F8X16[c*16+i]);
			OLED_SetPos(x,y+1);
			for(i=0;i<8;i++)
				WriteDat(F8X16[c*16+i+8]);
			x += 8;
			j++;
		}
}


void OLED_ShowHalfString(uint8_t x, uint8_t y, uint8_t ch[])
{
	uint8_t c = 0,i = 0,j = 0;
	while(ch[j] != '\0')
		{
			c = ch[j] - 32;
			if(x > 126)
			{
				x = 0;
				y++;
			}
			OLED_SetPos(x,y);
			for(i=0;i<6;i++)
				WriteDat(F6x8[c][i]);
			x += 6;
			j++;
		}
}

void OLED_ShowCN(uint8_t x, uint8_t y, uint8_t N)//(x:0~127, y:0~7) NΪ��Ӧ�ֿ⺺��
{
	uint8_t wm=0;
	uint16_t  adder=32*N;
	OLED_SetPos(x , y);
	for(wm = 0;wm < 16;wm++)
	{
		WriteDat(F16x16[adder]);
		adder += 1;
	}
	OLED_SetPos(x,y + 1);
	for(wm = 0;wm < 16;wm++)
	{
		WriteDat(F16x16[adder]);
		adder += 1;
	}
}

//--------------------------------------------------------------
// ����      : void OLED_DrawBMP(unsigned char x0,unsigned char y0,unsigned char x1,unsigned char y1,unsigned char BMP[]);
// ����˵��     : x0,y0 -- ��ʼ������(x0:0~127, y0:0~7); x1,y1 -- ���Խ���(������)������(x1:1~128,y1:1~8)
// Description    : ��ʾBMPλͼ
//--------------------------------------------------------------
void OLED_DrawBMP(uint8_t x0,uint8_t y0,uint8_t x1,uint8_t y1,uint8_t BMP[])
{
	uint16_t j=0;
	uint8_t x,y;

  if(y1%8==0)
		y = y1/8;
  else
		y = y1/8 + 1;
	for(y=y0;y<y1;y++)
	{
		OLED_SetPos(x0,y);
    for(x=x0;x<x1;x++)
		{
			WriteDat(BMP[j++]);
		}
	}
}
//OLEDͼƬ�л���ʾ����
void OLED_DrawBMP_Test(void)
{
	
		OLED_Fill(0x00); //��ȫ��

		OLED_DrawBMP(0,0,128,8,BMP1);

		delay_ms(3000);
		OLED_Fill(0x00); //��ȫ��
		
		OLED_DrawBMP(20,0,101,8,BMP2);  //ͼƬ��ʾ(ͼƬ��ʾ���ã����ɵ��ֱ�ϴ󣬻�ռ�ý϶�ռ䣬FLASH�ռ�8K��������)
		delay_ms(3000);
		
		OLED_Fill(0x00); //��ȫ��
		
		OLED_DrawBMP(20,0,114,8,BMP3);  //ͼƬ��ʾ(ͼƬ��ʾ���ã����ɵ��ֱ�ϴ󣬻�ռ�ý϶�ռ䣬FLASH�ռ�8K��������)
		delay_ms(3000);
}


///**
// * ����   		    OLEDˮƽ������ʾ
// * ����˵��   		start_page  : PAGE0~PAGE7
// * ����˵��       end_page  :  PAGE0~PAGE7    (end_page�������start_page)
// * ����˵��       frame  : 0~7
// * ����˵��      	dir  : 0 �ҹ�   1 ���
// * @retval      	��
// */
//void OLED_Scrolling_Display(uint8_t start_page, uint8_t end_page,uint8_t frame,uint8_t dir)
//{
//    WriteCmd(0x2E);	/* ֹͣ���� ���ú�,RAM������Ҫ����д�� */
//    
//    WriteCmd(dir ? 0x26 : 0x27);	/* 0,���ҹ��� 1,�����ƶ� */
//    WriteCmd(0x00);            		/* ���Ϳ��ֽ� ����Ϊ0x00���� */
//    WriteCmd(start_page & 0x07);	/* ��ʼҳ��ַ */
//    WriteCmd(frame & 0x07); 		/* ���ù�������ʱ����,֡Ϊ��λ */
//    WriteCmd(end_page & 0x07);	/* ��ֹҳ��ַ */

//    WriteCmd(0x00);   /* ���Ϳ��ֽ� ����Ϊ0x00���� */
//    WriteCmd(0xFF);   /* ���Ϳ��ֽ� ����Ϊ0xFF���� */
//    WriteCmd(0x2F);   /* �������� ��ֹ��RAM����,�ı�ˮƽ�������� */
//}

