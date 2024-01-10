#include <STC15F2K60S2.H>
#include <intrins.h>
#include "Sys.h"

void delay_us(uint32_t us)
{
    uint16_t x;
    for(x=us;x>0;x--)
    {
	_nop_();
	_nop_();
	_nop_();
    }
}

void delay_ms(uint32_t ms)
{
    uint8_t i, j;
    uint16_t x;
    for(x=ms;x>0;x--)
    {
    _nop_();
	_nop_();
	_nop_();
	i = 11;
	j = 190;
	do
	{
		while (--j);
	} while (--i);
    }
}
