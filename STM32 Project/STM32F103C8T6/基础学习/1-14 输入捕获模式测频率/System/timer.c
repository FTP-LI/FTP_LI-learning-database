#include "stm32f10x.h"                  // Device header

//extern int16_t Num;//�ⲿ���ñ���Num

void Timer_Init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);//����ʱ��TIM2
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//����GPIOA��ʱ��
	
	GPIO_InitTypeDef GPIO_InitStructure;//��ʼ��GPIO
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//��������ģʽ�������Ϊ�ߵ�ƽ������Ϊ�͵�ƽʱ������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;//ѡ��GPIOA^0
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);//������GPIOA
	
	TIM_ETRClockMode2Config(TIM2,TIM_ExtTRGPSC_OFF,TIM_ExtTRGPolarity_NonInverted,0x00);//ʹ���ⲿʱ��,����Ƶ���ߵ�ƽ�������������˲���
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision =TIM_CKD_DIV1;//ʱ�ӷ�ƵģʽΪһ��Ƶ������Ƶ��
	TIM_TimeBaseInitStructure.TIM_CounterMode =TIM_CounterMode_Up;//ʱ�Ӽ���ģʽΪ���ϼ���ģʽ
	TIM_TimeBaseInitStructure.TIM_Period = 10 - 1;//�Զ���װ������10�Σ���1����Ϊ����1��ƫ��
	TIM_TimeBaseInitStructure.TIM_Prescaler = 1 - 1;//������Ԥ��Ƶ
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter =0;//�ظ���������ӦTIM2Ϊͨ�ü�������û���ظ�������������
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);
	
	TIM_ClearFlag(TIM2,TIM_FLAG_Update);//�ֶ���������жϱ�־λ����ʼ�������̽����жϣ����³�ʼ����Ϊ1
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);//�����жϣ����������жϵ�NVIC��ͨ·
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//NVIC���ȼ�����
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel=TIM2_IRQn;//�����ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;//ʹ���ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;//��ռ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=1;//��Ӧ���ȼ�
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_Cmd(TIM2, ENABLE);//����TIM2��ʱ��
}

uint16_t Timer_GetCounter(void)
{
	return TIM_GetCounter(TIM2);
}

/*
void TIM2_IRQHandler(void)//TIM2�жϺ���
{
	if(TIM_GetITStatus(TIM2,TIM_IT_Update)==SET)//��ȡTIM2��ʱ�Ӹ��±�׼λ
	{
		TIM_ClearITPendingBit(TIM2,TIM_IT_Update);//���TIM2��ʱ�Ӹ��±�׼λ
	}
}	
*/
