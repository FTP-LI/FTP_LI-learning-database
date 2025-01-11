#include <STC15F2K60S2.H>
#include "Delay.h"
#include "F2481.h"
#include "DS18B20.h"
#include "BUZZER.h"

extern uint16_t DS18B20_Tempreature;


void main()
{
    uint8_t TMAX=28;
    uint8_t HT,LT;//分别对应十位，个位
    uint8_t i=0;
    BUZZER_OFF();
    while(1)
    {
        DS18B20_Get_Tempreature();
        HT=DS18B20_Tempreature/100;
        LT=DS18B20_Tempreature/10%10;
        F24811_display(2,HT);
        delay_ms(1);
        F24811_display(3,LT);
        delay_ms(1);
        F24811_display(4,12);
        delay_ms(1);
        if(HT*10+LT>=TMAX)
        {
            BUZZER_ON();
        }
        else
        {
            BUZZER_OFF();
        }
    }
}
