#include <STC15F2K60S2.H>
#include "DS18B20.h"
#include "Delay.h"
#include "Sys.h"

sbit DS = P1^4;//�źŽӿ�

uint16_t DS18B20_Tempreature = 0;       //���DS18B20��õ��¶�
uint16_t Timer0_Count = 0;              //���ڶ�ʱ��0����
uint8_t Warming_Flag = 0;             //�¶Ⱦ�����־λ������Ԥ���¶�ʱΪ1
uint16_t Tempreature_Max = 210;         //�����¶Ⱦ�����־

//ͨ����������DS18B20дһ���ֽ�
void Write_DS18B20(uint8_t dat)
{
	unsigned char i;
	for(i=0;i<8;i++)
	{
		DS = 0;
		DS = dat&0x01;
		delay_us(60);
		DS = 1;
		dat >>= 1;
	}
	delay_us(60);
}

//��DS18B20��ȡһ���ֽ�
uint8_t Read_DS18B20(void)
{
	uint8_t i;
	uint8_t dat;
  
	for(i=0;i<8;i++)
	{
		DS = 0;
		dat >>= 1;
		DS = 1;
		if(DS)
		{
			dat |= 0x80;
		}	    
		delay_us(60);
	}
	return dat;
}

//DS18B20�豸��ʼ��
bit init_ds18b20(void)
{
  	bit initflag = 0;
  	
  	DS = 1;
  	delay_us(72);
  	DS = 0;
  	delay_us(500);
  	DS = 1;
  	delay_us(60); 
    initflag = DS;     
  	delay_us(60);
  
  	return initflag;
}

void DS18B20_Get_Tempreature(void)      
{
    unsigned char LSB,MSB;
    init_ds18b20();     //��λ
    Write_DS18B20(0xCC);    //����ROMָ��
    Write_DS18B20(0x44);    //��ʼ�¶�ת��
    init_ds18b20();         //��λ
    Write_DS18B20(0xCC);    //����ROMָ��
    Write_DS18B20(0xBE);    //��ȡ�¶�
    LSB = Read_DS18B20();   
    MSB = Read_DS18B20();
    init_ds18b20();         //��λ
    
    DS18B20_Tempreature = (MSB<<8) | LSB;       //�Զ�ȡ���¶Ƚ��д���
    if((DS18B20_Tempreature & 0xF800) == 0x000) //�ж��¶�Ϊ��
    {
        DS18B20_Tempreature = (DS18B20_Tempreature >> 4) *10;           //����ȡ���¶ȷŴ�ʮ��,����Ϊ26.5�ȣ���������¶�Ϊ265
        DS18B20_Tempreature = DS18B20_Tempreature + (LSB & 0x0f)*0.625; 
    }

}






