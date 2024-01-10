#include <STC15F2K60S2.H>
#include "OLED.h"
#include "Delay.h"
#include "TM1637.h"
#include "DS18B20.h"
#include "BUZZER.h"
#include "LED.h"
#include "KEY.h"

extern uint16_t DS18B20_Tempreature;

void main()
{
    uint8_t TMAX=25,TMIN=12;
    uint8_t HT,LT,PT,FLAG,KEY;//分别对应十位，个位，小数位
    BUZZER_OFF();
    while(1)
    {
        DS18B20_Get_Tempreature();
        HT=DS18B20_Tempreature/100;
        FLAG=DS18B20_Tempreature/10;
        LT=FLAG%10;
        PT=DS18B20_Tempreature%10;
        TM1637_Display(HT,LT,PT,12,1);
        KEY=key_scan();
        if(KEY==1)
        {
            TMAX++;
        }
        else if(KEY==2)
        {
            TMAX--;
        }
        else if(KEY==3)
        {
            TMIN++;
        }
        else if(KEY==4)
        {
            TMIN--;
        }
        if(FLAG>=TMAX||FLAG<=TMIN)
        {
            BUZZER_ON();
            LED_Falsh();
            LED_Falsh();
            BUZZER_OFF();
        }
        else
        {
            BUZZER_OFF();
            LED_OFF();
        }
        delay_ms(1500);
    }
}
