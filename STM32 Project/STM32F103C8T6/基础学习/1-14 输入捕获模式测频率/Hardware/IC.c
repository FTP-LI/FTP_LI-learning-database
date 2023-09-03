#include "stm32f10x.h"                  // Device header

void IC_Init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);//����ʱ��TIM2
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//����GPIOA��GPIO��ʼ��
		
	GPIO_InitTypeDef GPIO_Iitstructure;
	GPIO_Iitstructure.GPIO_Mode = GPIO_Mode_IPU;//������������ģʽ��
	GPIO_Iitstructure.GPIO_Pin = GPIO_Pin_6;//����6
	GPIO_Iitstructure.GPIO_Speed = GPIO_Speed_50MHz;//�����ٶ�50MHz
	GPIO_Init(GPIOA,&GPIO_Iitstructure);
	
	TIM_InternalClockConfig(TIM3);//ʹ���ڲ�ʱ��(��ʼ��ʱ����Ԫ)
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision =TIM_CKD_DIV1;//ʱ�ӷ�ƵģʽΪһ��Ƶ������Ƶ��
	TIM_TimeBaseInitStructure.TIM_CounterMode =TIM_CounterMode_Up;//ʱ�Ӽ���ģʽΪ���ϼ���ģʽ
	TIM_TimeBaseInitStructure.TIM_Period = 65536 - 1;//����ARR��ֵ
	TIM_TimeBaseInitStructure.TIM_Prescaler = 72 - 1;//����PSC��ֵ
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter =0;//�ظ���������ӦTIM2Ϊͨ�ü�������û���ظ�������������
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);
	
	TIM_ICInitTypeDef TIM_ICIintSturcture; //��ʼ�����벶��Ԫ
	TIM_ICIintSturcture.TIM_Channel = TIM_Channel_1;//ѡ��ͨ��һ
	TIM_ICIintSturcture.TIM_ICFilter = 0xF;//�˲������ƴ�ģʽ
	TIM_ICIintSturcture.TIM_ICPolarity = TIM_ICPolarity_Rising;//���ؼƴ�ģʽ
	TIM_ICIintSturcture.TIM_ICPrescaler = TIM_ICPSC_DIV1;//����Ƶ
	TIM_ICIintSturcture.TIM_ICSelection = TIM_ICSelection_DirectTI;//ѡ��ֱ��ͨ��
	TIM_ICInit(TIM3,&TIM_ICIintSturcture);
	
	TIM_SelectInputTrigger(TIM3,TIM_TS_TI1FP1);//����TRGI�Ĵ���ԴΪTI1FP1
	
	TIM_SelectSlaveMode(TIM3,TIM_SlaveMode_Reset);//���ô�ģʽΪReset,�Զ���װ
	
	TIM_Cmd(TIM3,ENABLE);//����TM3��ʱ��
}
uint32_t IC_GetFreq(void)
{
	return 1000000 / (TIM_GetCapture1(TIM3)+1);
}
