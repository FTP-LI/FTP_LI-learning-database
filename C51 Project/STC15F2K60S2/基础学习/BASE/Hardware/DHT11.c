#include "DHT11.h"
#include <STC15F2K60S2.H>
#include "Delay.h"

sbit DHT11_data = P1^3;				//DHT11数据口用P1.3

uint8_t HmdHighDec,TmpHighDec,HmdLowDec,TmpLowDec;//用户变量通过extern调用

uint8_t dht1byte;
uint8_t HmdHigh,HmdLow,TmpHigh,TmpLow;
uint8_t TmpH,TmpL,HmdH,HmdL,check,numcheck;

void DHT11_Start(void)
{
	DHT11_data = 1;							    //总线拉高，开始信号
	delay_us(10);								//稍作延时
	DHT11_data = 0;							    //总线拉低
	delay_ms(20);								//总线拉低需大于18毫秒
	DHT11_data = 1;							    //发送开始信号后，拉高电平20-40us
	delay_us(32);		
}

uint8_t DHT11_Byte(void)//接收1个字节
{
	uint8_t i,temp;
	for(i = 0;i < 8;i++)
	{
		while(!DHT11_data);				        //等待50us的低电平信号结束
		delay_us(32);
		temp = 0;
		if(DHT11_data == 1)	temp =1;
		while(DHT11_data);
		dht1byte <<= 1;						    //接收的数据高位在前
		dht1byte |= temp;
	}
	return dht1byte;
}

void DHT11_Receive(void)
{
	DHT11_Start();
	DHT11_data=1;
	delay_us(1);
	if(!DHT11_data)							    //判断是否有低电平响应信号
	{
		while(!DHT11_data);				        //等待80us低电平响应信号结束
		while(DHT11_data);				        //等待80us高电平信号结束
		HmdH = DHT11_Byte();
		HmdL = DHT11_Byte();
		TmpH = DHT11_Byte();
		TmpL = DHT11_Byte();
		check = DHT11_Byte();
		DHT11_data = 0;						    //数据接收完成，拉低总线50us
		delay_us(56);									
		DHT11_data = 1;						    //拉高总线，进入空闲状态
		numcheck = HmdH + HmdL + TmpH + TmpL;   //校验和
		if(numcheck == check)			        //判断检验和和校验数据是否匹配
		{
			HmdHigh = HmdH;
			HmdLow  = HmdL;
			TmpHigh = TmpH;
			TmpLow	= TmpL;
			check = numcheck;
		}
	}
}

void Tran_Dec(void)
{
	uint8_t i;
	uint16_t sum,temp;
	sum = 0;
	temp = 0;
	for(i = 0;i < 8;i++)
	{
		temp = HmdHigh & 0x01;
		switch (i)
		{
			case 7:sum += temp*128;break;
			case 6:sum += temp*64;break;
			case 5:sum += temp*32;break;
			case 4:sum += temp*16;break;
			case 3:sum += temp*8;break;
			case 2:sum += temp*4;break;
			case 1:sum += temp*2;break;
			case 0:sum += temp;break;
			default:break;
		}
		HmdHigh >>= 1;
	}
	HmdHighDec = sum;//湿度高八位
	sum = 0;
	temp = 0;
	for(i = 0;i < 8;i++)
	{
		temp = TmpHigh & 0x01;
		switch (i)
		{
			case 7:sum += temp*128;break;
			case 6:sum += temp*64;break;
			case 5:sum += temp*32;break;
			case 4:sum += temp*16;break;
			case 3:sum += temp*8;break;
			case 2:sum += temp*4;break;
			case 1:sum += temp*2;break;
			case 0:sum += temp;break;
			default:break;
		}
		TmpHigh >>= 1;
	}
	TmpHighDec = sum;//温度高八位
	sum = 0;
	temp = 0;
    for(i = 0;i < 8;i++)
	{
		temp = TmpLow & 0x01;
		switch (i)
		{
			case 7:sum += temp*128;break;
			case 6:sum += temp*64;break;
			case 5:sum += temp*32;break;
			case 4:sum += temp*16;break;
			case 3:sum += temp*8;break;
			case 2:sum += temp*4;break;
			case 1:sum += temp*2;break;
			case 0:sum += temp;break;
			default:break;
		}
		TmpLow >>= 1;
	}
	TmpLowDec = sum;//温度低八位
	sum = 0;
	temp = 0;
    for(i = 0;i < 8;i++)
	{
		temp = HmdLow & 0x01;
		switch (i)
		{
			case 7:sum += temp*128;break;
			case 6:sum += temp*64;break;
			case 5:sum += temp*32;break;
			case 4:sum += temp*16;break;
			case 3:sum += temp*8;break;
			case 2:sum += temp*4;break;
			case 1:sum += temp*2;break;
			case 0:sum += temp;break;
			default:break;
		}
		HmdLow >>= 1;
	}
	HmdLowDec = sum;//湿度低八位
}