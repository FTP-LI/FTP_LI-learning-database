#include "PWM.h"

void PWM0_Init(void)//������ΪPWM_0ģ���ʼ��
{
    CMOD = 0x04;                //T0��ʱ������PWM����
    CCON = 0x00;                //��ʹ���жϱ�־��CR��ɺ���һ
    CCAPM0 = 0x42;              //ʹ�ܱȽϹ��ܣ�����PWM���
    CL = CH = 0;                //��������ʼ��
    CCAP0L = CCAP0H = 0;        //�Ƚ�ֵ��ʼ��
    PCA_PWM0 = 0x00;            //PWM_0:�����ڰ�λPWM����
    AUXR1 = 0xCC;               //ʹ��PWM_0��һ��PWM����ӿڣ���P1.1����
    
    CR = 1;//ʹ��PWMģ��
    Timer0_Init();
}

void Timer0_Init(void)		//11.0592MHz
{
	AUXR |= 0x80;			//��ʱ��ʱ��1Tģʽ
	TMOD &= 0xF0;			//���ö�ʱ��ģʽ
	TL0 = 0xA0;				//���ö�ʱ��ʼֵ,864��Ƶϵ��
	TH0 = 0xCF;				//���ö�ʱ��ʼֵ
	TF0 = 0;				//���TF0��־
	TR0 = 1;				//��ʱ��0��ʼ��ʱ
}

/*PWMƵ�ʼ���
PWM=PCAʱ��Դ/256=SYSclk/x/256 ��ϵͳʱ��/��Ƶϵ��/256
��������ʹ��STC15F2K60S2 �Դ�ʱ��Դ���м��㼴11.0592MHz
�ɵ�Ҫ�õ�50Hz����20ms����PWM��� ����ϵ��Ϊ864����T0��ʱ���ĳ�ֵΪ65536-864=64672 ��0xFCA0
Ϊ�˼��T0ѡ��1Tģʽ��16λ�Զ���װ����ͨ��STC-ISP���ٵõ�����
*/

void Steer_SetAngle(uint8_t Angle)
{
    CCAP0H = 249 - Angle/180*25;
}
/*PWMռ�ձȼ���
PWMռ�ձ�=(256-CCAPn)/256 ������ʹ��CCAP0ģ���ΪCCAP0
���ڶ�� PWMռ�ձ�2.5%~12.5%��0~180��
��CCAP0�ķ�ΧΪ 249~224 �༭CCAP0H��CCAP0L�Ĵ���
*/