#include <REG51.H>
#include "Sys.h"

void delay_ms(uint32_t ms)
{                         
	uint32_t a;
	while(ms)
	{
		a=1800;
		while(a--);
		ms--;
	}
}