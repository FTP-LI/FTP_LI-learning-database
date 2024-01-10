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
    P0M0=0x00;              //设置P0.0为开漏输出模式
    
    Timer_Init();
    TR0=1;                  //使能定时器0
    ADC_Init();
    
    Rain=1;
    PWM=0;                  //引脚初始化
    while(1);
}


void timer0()interrupt 1 //定时器0为0.1ms一个周期
{
    TH0=(65536-100)/256;
    TL0=(65536-100)%256;//重装定时器
    
    if(Rain==0)
    {
        Flag++;
        if(Flag==2000)//转动周期
        {
            Flag=0;
            CO++;
            CO = CO%2;//构建双位状态机
        }
        if(CO==1)
        {
            SET=190;//大概20度
        }
        if(CO==0)
        {
            SET=180;//大概90度    
        }
    }
    else//第电平状态 
    {
        SET=194;
    }
    if(a>SET)                   //产生周期为20ms，高电平为0.5ms，舵机会转到0度 SET=195  
    PWM=1;                      //如果想反转方向，就需把第一个p10=0，第二个p10=-1
    else
    PWM=0;
    a--;
    if(a<1)a=200; 
}