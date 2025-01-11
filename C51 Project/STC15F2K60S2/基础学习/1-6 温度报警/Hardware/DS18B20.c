#include <STC15F2K60S2.H>
#include "DS18B20.h"
#include "Delay.h"
#include "Sys.h"

sbit DS = P1^4;//信号接口

uint16_t DS18B20_Tempreature = 0;       //存放DS18B20测得的温度
uint16_t Timer0_Count = 0;              //用于定时器0计数
uint8_t Warming_Flag = 0;             //温度警报标志位，超过预期温度时为1
uint16_t Tempreature_Max = 210;         //设置温度警报标志

//通过单总线向DS18B20写一个字节
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

//从DS18B20读取一个字节
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

//DS18B20设备初始化
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
    init_ds18b20();     //复位
    Write_DS18B20(0xCC);    //跳过ROM指令
    Write_DS18B20(0x44);    //开始温度转换
    init_ds18b20();         //复位
    Write_DS18B20(0xCC);    //跳过ROM指令
    Write_DS18B20(0xBE);    //读取温度
    LSB = Read_DS18B20();   
    MSB = Read_DS18B20();
    init_ds18b20();         //复位
    
    DS18B20_Tempreature = (MSB<<8) | LSB;       //对读取的温度进行处理
    if((DS18B20_Tempreature & 0xF800) == 0x000) //判断温度为正
    {
        DS18B20_Tempreature = (DS18B20_Tempreature >> 4) *10;           //将读取的温度放大十倍,假设为26.5度，则读出的温度为265
        DS18B20_Tempreature = DS18B20_Tempreature + (LSB & 0x0f)*0.625; 
    }

}






