#include <STC15F2K60S2.H>
#include "OLED.h"
#include "Delay.h"
#include "TM1637.h"
#include "KEY.h"
#include "DHT11.h"

extern uint8_t HmdHighDec,TmpHighDec,HmdLowDec,TmpLowDec;
uint8_t HH,HM,HL,TH,TM,TL;      //��Ӧ��λʪ����ʾ����λ�¶���ʾ�ĸ��е���λ

void datas_cout(void)
{
    DHT11_Receive();            //DHT11���ݽ���
    Tran_Dec();                 //����ת��
    HH=HmdHighDec/10;
    HM=HmdHighDec%10;
    HL=HmdLowDec/10;
    TH=TmpHighDec/10;
    TM=TmpHighDec%10;
    TL=TmpLowDec/10;            //��ʾ���ݴ���
}

void main()
{
    uint8_t KEY;//������־λ
    uint8_t N=0;//����״̬��
    uint32_t time=0,TIM=0;//�����ʱ
    datas_cout();                   //��ʼ���ݻ�ȡ
    while(1)
    {
        KEY=key_scan();//�������
        if(KEY==1)
        {
            N++;                        //״̬��ת
            N=N%2;                      //��2ȡ�ౣ֤����״̬��N=0��N=1
            delay_ms(500);
        }
        if(N==0)//�¶���ʾģʽ
        {
            TM1637_Display(TH,TM,TL,12,1);
        }
        else if(N==1)//ʪ����ʾģʽ
        {
            TM1637_Display(HH,HM,HL,16,1);
        }
        time++;
        if(time==2090)
        {
            time=0;
            TIM++;
            if(TIM==10000)
            {
               datas_cout();//����ˢ��
            }
        }//�����ʱЧ����������10s���5%
    }
}
