#include <REG51.H>
#include "intrins.h"
#include "OLED.h"
#include "Delay.h"
#include "TM1637.h"
#include "DHT11.h"


uint16_t datas [5];

void main()
{
    OLED_Init();
    OLED_ShowNum(1,1,000000,6);
}
