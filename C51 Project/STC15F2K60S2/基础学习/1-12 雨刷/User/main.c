#include "STC15F2K60S2.h"  
#include "INTRINS.H"
#include "Delay.h"
#include "ADC.h"

sbit PWM=P0^0;
sbit Rain=P0^1;

uint8_t a;
uint8_t SET;
uint8_t Flag;
uint8_t CO;

void Timer_Init()
{
    a = 200;
    SET = 194;
    P10=0;
    Flag=0;
    CO=0;
    TMOD = 0x11;
    TH0 = (65536-100)/256;
    TL0 = (65536-100)%256;
    EA = 1;
    ET0 = 1;
}

void main()
{
    P0M0=0x00;              //����P0.0Ϊ��©���ģʽ
    
    Timer_Init();
    TR0=1;                  //ʹ�ܶ�ʱ��0
    ADC_Init();
    
    Rain=1;
    PWM=0;                  //���ų�ʼ��
    while(1);
}


void timer0()interrupt 1 //��ʱ��0Ϊ0.1msһ������
{
    TH0=(65536-100)/256;
    TL0=(65536-100)%256;//��װ��ʱ��
    
    if(Rain==0)
    {
        Flag++;
        if(Flag==2000)//ת������
        {
            Flag=0;
            CO++;
            CO = CO%2;//����˫λ״̬��
        }
        if(CO==1)
        {
            SET=190;//���20��
        }
        if(CO==0)
        {
            SET=180;//���90��    
        }
    }
    else//�ڵ�ƽ״̬ 
    {
        SET=194;
    }
    if(a>SET)                   //��������Ϊ20ms���ߵ�ƽΪ0.5ms�������ת��0�� SET=195  
    PWM=1;                      //����뷴ת���򣬾���ѵ�һ��p10=0���ڶ���p10=-1
    else
    PWM=0;
    a--;
    if(a<1)a=200; 
}