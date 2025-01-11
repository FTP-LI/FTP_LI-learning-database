#include <STC15F2K60S2.H>
#include "Delay.h"
#include "F2481.h"

#define ADC_POWER 0x80
#define ADC_START 0x08
#define ADC_FLAG 0x10
#define ADC_SPEEDH 0x40                     //转换速度为180个时钟周期
#define ADRJ 0x20                           //ADRJ = 1;
#define AD_P1_0 0x00                        //P1.0为AD输入;

sfr PCON2 = 0x97;

void ADC_Init(void)
{
       P1ASF= 0x01;                         //设置P1为模拟输入端口  
       PCON2= PCON2 | ADRJ;           //ADRJ  = 1;
       ADC_CONTR= ADC_POWER | ADC_SPEEDH | AD_P1_0;
       //设置A/D电源开启、转换速度设置、A/D输入端口；
       delay_ms(100);//等待电源稳定    
}

unsigned int Gat_ADC(void)//电压=（5/1024=0.004882）*AD值
{
       uint16_t i,j,AD_Vaule;
       ADC_CONTR= ADC_CONTR | ADC_START;                 //开始转换
       while((ADC_CONTR& ADC_FLAG) != ADC_FLAG);    //等待转换标志置位
       i= ADC_RES;  //高两位
       j= ADC_RESL; //低八位
       i= i << 8;
       i= i | j;
       ADC_CONTR= ADC_CONTR & ~ADC_FLAG ;                   //清零转换标志位
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
            V=V*0.019607*10;//电压值转换,取三位*100
            D1=V/100;
            D2=(V/10)%10;
            D3=V%10;//数据转换
            F24811_display(2,D2);
            delay_ms(1);
            F24811_display(3,D3);
            delay_ms(1);
            F24811_display(4,20); 
            delay_ms(1);            
    }
}
