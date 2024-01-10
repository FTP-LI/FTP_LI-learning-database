#include "DHT11.h"
#include <STC15F2K60S2.H>
#include "Delay.h"

sbit DHT11_data = P1^3;				//DHT11���ݿ���P1.3

uint8_t HmdHighDec,TmpHighDec,HmdLowDec,TmpLowDec;//�û�����ͨ��extern����

uint8_t dht1byte;
uint8_t HmdHigh,HmdLow,TmpHigh,TmpLow;
uint8_t TmpH,TmpL,HmdH,HmdL,check,numcheck;

void DHT11_Start(void)
{
	DHT11_data = 1;							    //�������ߣ���ʼ�ź�
	delay_us(10);								//������ʱ
	DHT11_data = 0;							    //��������
	delay_ms(20);								//�������������18����
	DHT11_data = 1;							    //���Ϳ�ʼ�źź����ߵ�ƽ20-40us
	delay_us(32);		
}

uint8_t DHT11_Byte(void)//����1���ֽ�
{
	uint8_t i,temp;
	for(i = 0;i < 8;i++)
	{
		while(!DHT11_data);				        //�ȴ�50us�ĵ͵�ƽ�źŽ���
		delay_us(32);
		temp = 0;
		if(DHT11_data == 1)	temp =1;
		while(DHT11_data);
		dht1byte <<= 1;						    //���յ����ݸ�λ��ǰ
		dht1byte |= temp;
	}
	return dht1byte;
}

void DHT11_Receive(void)
{
	DHT11_Start();
	DHT11_data=1;
	delay_us(1);
	if(!DHT11_data)							    //�ж��Ƿ��е͵�ƽ��Ӧ�ź�
	{
		while(!DHT11_data);				        //�ȴ�80us�͵�ƽ��Ӧ�źŽ���
		while(DHT11_data);				        //�ȴ�80us�ߵ�ƽ�źŽ���
		HmdH = DHT11_Byte();
		HmdL = DHT11_Byte();
		TmpH = DHT11_Byte();
		TmpL = DHT11_Byte();
		check = DHT11_Byte();
		DHT11_data = 0;						    //���ݽ�����ɣ���������50us
		delay_us(56);									
		DHT11_data = 1;						    //�������ߣ��������״̬
		numcheck = HmdH + HmdL + TmpH + TmpL;   //У���
		if(numcheck == check)			        //�жϼ���ͺ�У�������Ƿ�ƥ��
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
	HmdHighDec = sum;//ʪ�ȸ߰�λ
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
	TmpHighDec = sum;//�¶ȸ߰�λ
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
	TmpLowDec = sum;//�¶ȵͰ�λ
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
	HmdLowDec = sum;//ʪ�ȵͰ�λ
}