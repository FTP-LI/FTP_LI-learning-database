#include "STC15F2K60S2.h" 
#include "INTRINS.H"
#include "Delay.h"

sbit bee=P0^0;
sbit conc=P0^1;

uint8_t Flag = 0;


void Sound(uint16_t frq)
{
    uint32_t time;
    if(frq != 1000)
    {
        time = 500000/((uint32_t)frq);
        bee = 1;
        delay_us(time);
        bee = 0;
        delay_us(time);
    }else
        delay_us(1000);
}

void Sound2(uint16_t time)
{
    bee = 1;
    delay_ms(time);
    bee = 0;
    delay_ms(time);
}

uint16_t tone[] ={250,262,294,330,350,393,441,495,525,589,661,700,786,882,990}; //����        
uint8_t music[]={5,5,6,5,8,7,5,5,6,5,9,8,5,5,12,10,8,7,6,11,
                  11,10,8,9,8,5,5,8,5,5,12,10,8,7,6,11,11,10,8,9,8    //��������
        };     
uint8_t time[] = {1,2,2,2,2,4,1,2,2,2,2,4,1,2,2,2,1,4,
                  4,1,2,2,2,2,4,1,2,4,1,2,2,2,1,4, 4,1,2,2,2,2,4,4        //����ʱ��
        }; 

void play_sound(void)
{
    uint32_t yanshi;
    uint16_t i,e;
    P1 = 0xF0;
    yanshi = 10;
     for(i=0;i<sizeof(music)/sizeof(music[0]);i++){
         P1 = ~P1;
        for(e=0;e<((uint16_t)time[i])*tone[music[i]]/yanshi;e++){
            Sound((uint32_t)tone[music[i]]);
            if(Flag == 1)
            {
                break;
            }
        }
        if(Flag == 1)
            {
                break;
            }
    }
}

void INT0_Init(void){
  IT0 = 1;   //ֻ�����½��ش���
  EX0 = 1;    //�ж��ⲿ�ж�0�ж�
  EA = 1;     //��������ж�
}

void INT0_handle(void) interrupt 0
    {
        Flag ++ ;
        Flag = Flag %2;
}

void main()
{
    INT0_Init();
    while(1)
    {
        if(Flag == 0)
        {
             P1 = 0x00;
        }
        if(Flag == 1)
        {
            play_sound();
        }
    }
}


