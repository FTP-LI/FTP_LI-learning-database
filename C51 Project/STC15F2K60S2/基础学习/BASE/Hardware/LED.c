#include <STC15F2K60S2.H>
#include "LED.h"
#include "Delay.h"


sbit LED = P0^1; 

void LED_ON(void)
{
    LED=0;
}

void LED_OFF(void)
{
    LED=1;
}

void LED_Falsh(void)
{
    LED=0;
    delay_ms(100);
    LED=1;
    delay_ms(100);
}
