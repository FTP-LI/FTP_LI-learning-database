#include <STC15F2K60S2.H>
#include "OLED.h"
#include "Delay.h"
#include "DS1302.h"

#define KEY1_PRESS	1
#define KEY2_PRESS  2
#define KEY_NOPRESS	0


// OLED_SCL=P0^0
// OLED_SDA=P0^1    //OLED
sbit BUZZER = P0^2;
sbit LED = P0^3;
// SCLK=P0^5
// DAT=P0^6
// RST=P0^7         //DS1302
sbit KEY1=P1^0;
sbit KEY2=P1^1;

extern uint8_t DS1302_Time[7];//�ꡢ�¡��ܡ��ա�ʱ���֡���
uint8_t Se=0,Mi=0;

uint8_t key_scan(void)
{
    if(KEY1==0||KEY2==0)
    {
        delay_ms(15);
        if(KEY1==0)
        {
            return KEY1_PRESS;
        }
        if(KEY2==0)
        {
            return KEY2_PRESS;
        }
    }
    return KEY_NOPRESS;
}

void OLED_ShowCout(void)
{
    OLED_ShowNum(1,5,DS1302_Time[4],2);
    OLED_ShowString(1,7,":");
    OLED_ShowNum(1,8,DS1302_Time[5],2);
    OLED_ShowString(1,10,":");
    OLED_ShowNum(1,11,DS1302_Time[6],2);
    OLED_ShowString(2,4,"Count Set:");
    OLED_ShowNum(3,6,Mi,2);
    OLED_ShowString(3,8,":");
    OLED_ShowNum(3,9,Se,2);
}

void LOUND_ON(void)
{
    BUZZER = 0;
    LED=0;
}

void LOUND_OFF(void)
{
    BUZZER = 1;
    LED=1;
}

void main()
{
    uint8_t Flag=0;
    uint8_t F,S,key;
    Ds1302_Init();
    OLED_Init();
    OLED_Clear();
    DS1302_Time[4]=0;
    DS1302_Time[5]=0;
    DS1302_Time[6]=0;
    DS1302_SetTime();//����ʱ��
    while(1)
    {
        key=key_scan();
        if(key==1)//���ö�ʱֵ
        {
            Se++;
            delay_ms(100);
            while(Se>=60)
            {
                Mi++;
                Se=Se-60;
            }
            Flag=0;
        }
        if(key==2)//ȷ����ʱֵ
        {
            while(Se>=60)
            {
                Mi++;
                Se=Se-60;
            }
            DS1302_Time[5]=0;
            DS1302_Time[6]=0;
            DS1302_SetTime();
            Flag=1;
        }
        if(Flag==1)//���뵹��ʱ
        {
            DS1302_ReadTime();//ˢ��ʱ��
            OLED_ShowCout();
            if(DS1302_Time[5]>=Mi&&DS1302_Time[6]>=Se)//�ж��Ƿ�ָ��ʱ��
            {
                OLED_ShowString(4,5,"TIME_GET!");
                for(F=0;F<3;F++)//����
                {
                    for(S=0;S<2;S++)
                    {
                        LOUND_ON();
                        delay_ms(200);
                        LOUND_OFF();
                        delay_ms(200);
                    }
                    delay_ms(800);
                }
                OLED_Clear();
                Flag=0;
                DS1302_Time[5]=0;
                DS1302_Time[6]=0;
                Se=0;
                Mi=0;
                DS1302_SetTime();//����ʱ��
            }
        }
        if(Flag==0)//�����趨ģʽ
        {
            DS1302_ReadTime();
            OLED_ShowCout();
        }
        
    }
}
