#include "stm32f10x.h"                  // Device header
#include "delay.h"
#include "OLED.h"
#include "Serial.h"

int main(void)
{
	OLED_Init();
	Serial_Init();
	
	Serial_Printf("Ը���������������\r\n");
	
	while(1)
	{
		
	}
}	
