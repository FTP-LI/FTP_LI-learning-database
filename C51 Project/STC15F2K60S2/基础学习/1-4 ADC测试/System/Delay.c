#include <REG51.H>
#include "Sys.h"

void delay_us(uint32_t us)
{
    while(--us);
}
void delay_ms(uint32_t ms)
{
   uint32_t i,j;
   for(i=ms;i>0;i--)
      for(j=110;j>0;j--);
}