#include "pwm.h"

//�������н���CCP2����г�ʼ����ӦP3^5~7������

void PWM_Init(void)
{
  P_SW1 &=0xCF; //�л�ΪCCPn_2������
  P_SW1 |=0x10; //CCP0_2=P35��CCP1_2=P36, CCP2_2=P37
  
  CMOD=0x0E;    //PWMʱ��0.375Mhz,1.46khz,�رռ�������ж�
  CCAPM0=0x02;  //PCA0,����PWMģʽ
  CCAPM1=0x02;  //PCA1,����PWMģʽ
  CCAPM2=0x02;  //PCA2,����PWMģʽ
  PCA_PWM0=0x00;//8λPWMģʽ��EPCOH=EPCOL=0
  PCA_PWM1=0x00;//8λPWMģʽ��EPCOH=EPCOL=0
  PCA_PWM2=0x00;//8λPWMģʽ��EPCOH=EPCOL=0
  
  CL=CH=0;      //PCA����ֵ����
  
  CCAP0H=CCAP0L=128-1; //��ʼ��ռ�ձ�Ϊ50%
  CCAP1H=CCAP1L=128-1; //��ʼ��ռ�ձ�Ϊ50%
  CCAP2H=CCAP2L=128-1; //��ʼ��ռ�ձ�Ϊ50%
  
  CCON=0x40;    //��ʼPCA���У��жϱ�־λ���� 
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

void Set_CCP2Pwm(uint8_t pwm)    //����PWMռ�ձ�
{
    CCAP0H=pwm;
    CCAP1H=pwm;
    CCAP2H=pwm;
}
