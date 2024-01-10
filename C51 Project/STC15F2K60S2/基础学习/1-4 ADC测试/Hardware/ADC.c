#include <REG51.H>
#include <intrins.h>
#include "Sys.h"
#include "ADC.h"
#include "Delay.h"

#define ADC_POWER 0x80
#define ADC_START 0x08
#define ADC_FLAG 0x10
#define ADC_SPEEDH 0x40                     //转换速度为180个时钟周期
#define ADRJ 0x20                           //ADRJ = 1;
#define AD_P1_0 0x00                        //P1.0为AD输入;

sfr ADC_CONTR = 0xbc;
sfr ADC_RES = 0xbd;
sfr ADC_RESL = 0xbE;
sfr P1ASF = 0x9D;
sfr PCON2 = 0x97;

void ADC_Init(void)
{
       P1ASF= 0x01;                         //设置P1为模拟输入端口  
       PCON2= PCON2 | ADRJ;           //ADRJ  = 1;
       ADC_CONTR= ADC_POWER | ADC_SPEEDH | AD_P1_0;
       //设置A/D电源开启、转换速度设置、A/D输入端口；
       delay_ms(5);//等待电源稳定    
    }

unsigned int Gat_ADC(void)
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