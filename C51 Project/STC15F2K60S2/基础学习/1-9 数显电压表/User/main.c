#include <STC15F2K60S2.H>
#include "Delay.h"
#include "TM1637.h"

#define ADC_POWER 0x80
#define ADC_START 0x08
#define ADC_FLAG 0x10
#define ADC_SPEEDH 0x40                     //ת���ٶ�Ϊ180��ʱ������
#define ADRJ 0x20                           //ADRJ = 1;
#define AD_P1_0 0x00                        //P1.0ΪAD����;

sfr PCON2 = 0x97;

void ADC_Init(void)
{
       P1ASF= 0x01;                         //����P1Ϊģ������˿�  
       PCON2= PCON2 | ADRJ;           //ADRJ  = 1;
       ADC_CONTR= ADC_POWER | ADC_SPEEDH | AD_P1_0;
       //����A/D��Դ������ת���ٶ����á�A/D����˿ڣ�
       delay_ms(100);//�ȴ���Դ�ȶ�    
}

unsigned int Gat_ADC(void)//��ѹ=��5/1024=0.004882��*ADֵ
{
       uint16_t i,j,AD_Vaule;
       ADC_CONTR= ADC_CONTR | ADC_START;                 //��ʼת��
       while((ADC_CONTR& ADC_FLAG) != ADC_FLAG);    //�ȴ�ת����־��λ
       i= ADC_RES;  //����λ
       j= ADC_RESL; //�Ͱ�λ
       i= i << 8;
       i= i | j;
       ADC_CONTR= ADC_CONTR & ~ADC_FLAG ;                   //����ת����־λ
       ADC_RES= 0;
       ADC_RESL= 0;
       AD_Vaule=i;
       return(AD_Vaule);
}

void main()
{
    uint8_t n,V,D1,D2,D3;
    uint8_t a[10];
    ADC_Init();
    while(1)
    {
        for(n=0;n<10;n++)
            {
               a[n]= Gat_ADC();
            }
        V=(a[0]+a[1]+a[2]+a[3]+a[4]+a[5]+a[6]+a[7]+a[8]+a[9])/10;
        V=V*0.019607*10;//��ѹֵת��,ȡ��λ*100
        D1=V/100;
        D2=(V/10)%10;
        D3=V%10;//����ת��
        TM1637_Display(D1,D2,D3,20,1);//��ʾ��ѹ
        delay_ms(1000);
    }
}
