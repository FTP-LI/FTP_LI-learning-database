#include "stm32f10x.h"                  // Device header
#include "OLED.h"
#include "Delay.h"
#include "PWM.h"

uint16_t i=0;
int main(void)
{
	PWM_Init();
//	OLED_Init();
	while(1)
	{
		for(i=0; i<100; i++)
		{
			PWM_SetCompare1(i);
			Delay_ms(10);
		}
		for(i=0; i<100; i++)
		{
			PWM_SetCompare1(100-i);
			Delay_ms(10);
		}
	}
}
