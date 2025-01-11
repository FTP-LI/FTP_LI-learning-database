#include <STC15F2K60S2.H>
#include "Delay.h"

sbit LED1=P0^0;
sbit LED2=P0^1;
sbit LED3=P0^3;
sbit LED4=P0^2;

uint8_t Flag = 0;

void INT0_Init(void){
  IT0 = 1;   //只允许下降沿触发
  EX0 = 1;    //中断外部中断0中断
  EA = 1;     //允许存在中断
}

void INT0_handle(void) interrupt 0
    {
        Flag ++ ;
        Flag = Flag %2;
}

void main()
{
    uint16_t time = 0;
    INT0_Init();
    while(1)
    {
        if(time == 600)
        {
            time = 0;
            Flag ++ ;
            Flag = Flag %2;
        }
        if(Flag == 0)
        {
            LED1 = 0;
            LED2 = 1;
            LED3 = 0;
            LED4 = 1;
        }
        if(Flag == 1)
        {
            LED1 = 1;
            LED2 = 0;
            LED3 = 1;
            LED4 = 0;
        } 
        delay_ms(10);
        time++;
    }
}
