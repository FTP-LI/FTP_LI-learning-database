#include "pwm.h"

//本函数中仅对CCP2组进行初始化对应P3^5~7号引脚

void PWM_Init(void)
{
  P_SW1 &=0xCF; //切换为CCPn_2组引脚
  P_SW1 |=0x10; //CCP0_2=P35，CCP1_2=P36, CCP2_2=P37
  
  CMOD=0x0E;    //PWM时钟0.375Mhz,1.46khz,关闭计数溢出中断
  CCAPM0=0x02;  //PCA0,设置PWM模式
  CCAPM1=0x02;  //PCA1,设置PWM模式
  CCAPM2=0x02;  //PCA2,设置PWM模式
  PCA_PWM0=0x00;//8位PWM模式，EPCOH=EPCOL=0
  PCA_PWM1=0x00;//8位PWM模式，EPCOH=EPCOL=0
  PCA_PWM2=0x00;//8位PWM模式，EPCOH=EPCOL=0
  
  CL=CH=0;      //PCA计数值清零
  
  CCAP0H=CCAP0L=128-1; //初始化占空比为50%
  CCAP1H=CCAP1L=128-1; //初始化占空比为50%
  CCAP2H=CCAP2L=128-1; //初始化占空比为50%
  
  CCON=0x40;    //开始PCA阵列，中断标志位清零 
}

void Set_CCP0_2Pwm(uint8_t pwm)//P35
{
    CCAP0H=pwm;
}
void Set_CCP1_2Pwm(uint8_t pwm)//P36
{
    CCAP1H=pwm;
}
void Set_CCP2_2Pwm(uint8_t pwm)//P37
{
    CCAP2H=pwm;
}

void Set_CCP2Pwm(uint8_t pwm)    //设置PWM占空比
{
    CCAP0H=pwm;
    CCAP1H=pwm;
    CCAP2H=pwm;
}
