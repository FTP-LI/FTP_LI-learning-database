#include <STC15F2K60S2.H>
#include "Delay.h"

sbit Part1=P0^1;
sbit Part2=P0^3;

sbit KEY=P0^2;

#define KEY_PRESS	1
#define KEY_NOPRESS	0

uint8_t Flag=0,i=0,j=0;

void Part1_ON(void)
{
    Part1 = 0;
}
void Part1_OFF(void)
{
    Part1 = 1;
}

void Part2_ON(void)
{
    Part2 = 0;
}
void Part2_OFF(void)
{
    Part2 = 1;
}

uint8_t signkey(void)
{
    if(KEY==0)
    {
        delay_ms(15);
        if(KEY==0)
        {
            return KEY_PRESS;
        }
    }
    return KEY_NOPRESS;
}


void main()
{
    uint8_t key;
    while(1)
    {
        key = signkey();
        if(key==1)
        {
           delay_ms(200);
           j=0;
           i=0;
           Flag++;
           Flag=Flag%2;//构建状态机，双状态
        }
        if(Flag==0)//车行道亮绿，人行道红
        {
            Part1_ON();
            Part2_OFF();
        }
        if(Flag==1)//与上状态相反
        {
            Part1_OFF();
            Part2_ON();
        }
        
        j++;
        delay_ms(10);
        if(j>=200)
        {
        j=0;
        i++;
        if(i==2)
        {
        i=0;
        Flag++;
        Flag=Flag%2;//构建状态机，双状态
        }
        }
    }
}
