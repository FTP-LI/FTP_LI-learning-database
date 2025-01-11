#include <STC15F2K60S2.H>
#include "BUZZER.h"
#include "Delay.h"

sbit BUZZER = P0^0;

void BUZZER_ON(void)
{
    BUZZER = 0;
}

void BUZZER_OFF(void)
{
    BUZZER = 1;
}

void BUZZER_Flash(void)
{
    BUZZER = 0;
    delay_ms(100);
    BUZZER = 1;
    delay_ms(100);
}
