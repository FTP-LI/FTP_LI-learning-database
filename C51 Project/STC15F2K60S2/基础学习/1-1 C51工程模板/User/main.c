#include <REG51.H>
#include "Delay.h"
#include "OLED.h"

void main()
{
    OLED_Init();
    OLED_ShowNum(1,1,000000,6);
}
