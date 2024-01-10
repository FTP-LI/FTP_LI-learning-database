#include <REG51.H>
#include "intrins.h"
#include "OLED.h"
#include "Delay.h"

void main()
{
    OLED_Init();
    OLED_ShowString(1,1,"Hello World!");
    OLED_ShowCN(0,2,0);
    OLED_ShowCN(16,2,1);
    OLED_ShowCN(32,2,2);
    OLED_ShowCN(48,2,3);
    OLED_ShowCN(64,2,4);
    OLED_ShowCN(80,2,5);
    OLED_ShowCN(96,2,6);
    OLED_ShowCN(112,2,7);
}
