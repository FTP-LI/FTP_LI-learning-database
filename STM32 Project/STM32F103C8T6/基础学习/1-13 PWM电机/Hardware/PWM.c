#include "stm32f10x.h"                  // Device header

void PWM_Init(void)
{
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);//����ʱ��TIM2
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//����GPIOA��GPIO��ʼ��
		
		GPIO_InitTypeDef GPIO_Iitstructure;
		GPIO_Iitstructure.GPIO_Mode = GPIO_Mode_AF_PP;//�����������ģʽ������ʱ���������ţ�ʹ�ø������ģʽ��
		GPIO_Iitstructure.GPIO_Pin = GPIO_Pin_2;//����2
		GPIO_Iitstructure.GPIO_Speed = GPIO_Speed_50MHz;//�����ٶ�50MHz
		GPIO_Init(GPIOA,&GPIO_Iitstructure);
		
		
		TIM_InternalClockConfig(TIM2);//ʹ���ڲ�ʱ��
	
		TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
		TIM_TimeBaseInitStructure.TIM_ClockDivision =TIM_CKD_DIV1;//ʱ�ӷ�ƵģʽΪһ��Ƶ������Ƶ��
		TIM_TimeBaseInitStructure.TIM_CounterMode =TIM_CounterMode_Up;//ʱ�Ӽ���ģʽΪ���ϼ���ģʽ
		TIM_TimeBaseInitStructure.TIM_Period = 100 - 1;//����ARR��ֵ
		TIM_TimeBaseInitStructure.TIM_Prescaler = 36 - 1;//����PSC��ֵ
		TIM_TimeBaseInitStructure.TIM_RepetitionCounter =0;//�ظ���������ӦTIM2Ϊͨ�ü�������û���ظ�������������
		TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);
		
		TIM_OCInitTypeDef TMO_OCInitstructure;
		TIM_OCStructInit(&TMO_OCInitstructure);//��ʼ������Ƚ�������
		TMO_OCInitstructure.TIM_OCMode = TIM_OCMode_PWM1 ;//��������Ƚ�ģʽΪPWM1
		TMO_OCInitstructure.TIM_OCPolarity = TIM_OCPolarity_High ;//OC����Ϊ�߼��ԣ�REF����ֱ���������REF��Чʱ����ߵ�ƽ
		TMO_OCInitstructure.TIM_OutputState = TIM_OutputState_Enable ;//������������ʹ�ܣ�
		TMO_OCInitstructure.TIM_Pulse = 0;//�趨CCR��ֵ
		TIM_OC3Init(TIM2,&TMO_OCInitstructure);
		
		
		
	TIM_Cmd(TIM2, ENABLE);//����TIM2��ʱ��
}
}
void PWM_SetCompare3(uint16_t Compare)
{
	TIM_SetCompare3(TIM2,Compare);//�����й����и���CCR��ֵ
}
/*
PWMƵ�� Freq=ʱ��Ƶ��/��PSC+1��/��ARR+1��
PWMռ�ձ� Duty=CCR/(ARR+1)
PWM�ֱ��� Reso=1/(ARR+1)
*/
