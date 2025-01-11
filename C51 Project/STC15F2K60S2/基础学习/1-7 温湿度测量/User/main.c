#include <STC15F2K60S2.H>
#include "Delay.h"
#include "F2481.h"
#include "DHT11.h"

extern uint8_t HmdHighDec,TmpHighDec,HmdLowDec,TmpLowDec;
uint8_t HH,HM,TH,TM,FLAG=0,time=0;      //��Ӧ��λʪ����ʾ����λ�¶���ʾ�ĸ��е���λ

void datas_cout(void)
{
    DHT11_Receive();            //DHT11���ݽ���
    Tran_Dec();                 //����ת��
    HH=HmdHighDec/10;
    HM=HmdHighDec%10;
    TH=TmpHighDec/10;
    TM=TmpHighDec%10;
}

void INT0_Init(void){
  IT0 = 1;   //ֻ�����½��ش���
  EX0 = 1;    //�ж��ⲿ�ж�0�ж�
  EA = 1;     //��������ж�
}

void INT0_handle(void) interrupt 0
    {
        FLAG++;
        FLAG = FLAG%2;
        time = 0;
        delay_ms(10);
}

void main()
{
    uint8_t N=0;//����״̬��
    uint32_t TIM=0;//�����ʱ
    datas_cout();                   //��ʼ���ݻ�ȡ
    INT0_Init();
    while(1)
    {
        TIM ++;
        time ++;
        if(time == 3500)
        {
            time = 0;
            FLAG++;
            FLAG = FLAG%2;
        }
        if(TIM == 600)
        {
            datas_cout();
            TIM = 0;
        }
        if(FLAG == 0)//�¶�
        {
            F24811_display(2,TH);
            delay_ms(1);
            F24811_display(3,TM);
            delay_ms(1);
            F24811_display(4,16);
            delay_ms(1);
        }
        if(FLAG == 1)//ʪ��
        {
            F24811_display(2,HH);
            delay_ms(1);
            F24811_display(3,HM);
            delay_ms(1);
            F24811_display(4,12);
            delay_ms(1);
        }
    }
}
