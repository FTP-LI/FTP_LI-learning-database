#include <REG51.H>
#include <intrins.h>
#include "Sys.h"
#include "ADC.h"
#include "Delay.h"

#define ADC_POWER 0x80
#define ADC_START 0x08
#define ADC_FLAG 0x10
#define ADC_SPEEDH 0x40                     //ת���ٶ�Ϊ180��ʱ������
#define ADRJ 0x20                           //ADRJ = 1;
#define AD_P1_0 0x00                        //P1.0ΪAD����;

sfr ADC_CONTR = 0xbc;
sfr ADC_RES = 0xbd;
sfr ADC_RESL = 0xbE;
sfr P1ASF = 0x9D;
sfr PCON2 = 0x97;

void ADC_Init(void)
{
       P1ASF= 0x01;                         //����P1Ϊģ������˿�  
       PCON2= PCON2 | ADRJ;           //ADRJ  = 1;
       ADC_CONTR= ADC_POWER | ADC_SPEEDH | AD_P1_0;
       //����A/D��Դ������ת���ٶ����á�A/D����˿ڣ�
       delay_ms(5);//�ȴ���Դ�ȶ�    
    }

unsigned int Gat_ADC(void)
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