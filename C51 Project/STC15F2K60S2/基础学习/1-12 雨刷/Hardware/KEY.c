#include <STC15F2K60S2.H>
#include "KEY.h"
#include "Delay.h"

sbit KEY1=P0^0;
sbit KEY2=P0^2;
sbit KEY3=P0^3;
sbit KEY4=P0^4;

#define KEY1_PRESS	1
#define KEY2_PRESS	2
#define KEY3_PRESS	3
#define KEY4_PRESS	4
#define KEY_NOPRESS	0

uint8_t signkey(void)
{
    if(KEY1==0)
    {
        delay_ms(15);
        if(KEY1==0)
        {
            return KEY1_PRESS;
        }
    }
    return KEY_NOPRESS;
}

uint8_t key_scan(void)
{
	if(KEY1==0||KEY2==0||KEY3==0||KEY4==0)//任意按键按下
	{
		delay_ms(15);//消抖
		if(KEY1==0)
			return KEY1_PRESS;
		else if(KEY2==0)
			return KEY2_PRESS;
		else if(KEY3==0)
			return KEY3_PRESS;
		else if(KEY4==0)
			return KEY4_PRESS;
        else
            return KEY_NOPRESS;
	}
    return KEY_NOPRESS;
}