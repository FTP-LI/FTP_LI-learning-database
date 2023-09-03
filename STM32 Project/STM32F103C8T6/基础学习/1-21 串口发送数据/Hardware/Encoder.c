#include "stm32f10x.h"                  // Device header

void Encoder_Init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);//����ʱ��TIM3
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//����GPIOA��GPIO��ʼ��
		
	GPIO_InitTypeDef GPIO_Iitstructure;
	GPIO_Iitstructure.GPIO_Mode = GPIO_Mode_IPU;//������������ģʽ
	GPIO_Iitstructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;//����6��7
	GPIO_Iitstructure.GPIO_Speed = GPIO_Speed_50MHz;//�����ٶ�50MHz
	GPIO_Init(GPIOA,&GPIO_Iitstructure);
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision =TIM_CKD_DIV1;//ʱ�ӷ�ƵģʽΪһ��Ƶ������Ƶ��
	TIM_TimeBaseInitStructure.TIM_CounterMode =TIM_CounterMode_Up;//ʱ�Ӽ���ģʽΪ���ϼ���ģʽ
	TIM_TimeBaseInitStructure.TIM_Period = 65536 - 1;//����ARR��ֵ�������̼���
	TIM_TimeBaseInitStructure.TIM_Prescaler = 1 - 1;//����PSC��ֵ�������з�Ƶ
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter =0;//�ظ���������ӦTIM2Ϊͨ�ü�������û���ظ�������������
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);//ʱ����Ԫ����
	
	TIM_ICInitTypeDef TIM_ICIintSturcture; //��ʼ�����벶��Ԫ
	TIM_ICStructInit(&TIM_ICIintSturcture);
	TIM_ICIintSturcture.TIM_Channel = TIM_Channel_1;//ѡ��ͨ��һ
	TIM_ICIintSturcture.TIM_ICFilter = 0xF;//�˲������ƴ�ģʽ
	TIM_ICInit(TIM3,&TIM_ICIintSturcture);//����ͨ��һ
	TIM_ICIintSturcture.TIM_Channel = TIM_Channel_2;//ѡ��ͨ����
	TIM_ICIintSturcture.TIM_ICFilter = 0xF;//�˲������ƴ�ģʽ
	TIM_ICInit(TIM3,&TIM_ICIintSturcture);//����ͨ����
	
	TIM_EncoderInterfaceConfig( TIM3, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);//���ñ������ӿڣ�TIM_ICPolarity_Rising���Բ���ת,��ת��һ���Ըı��������
	
	TIM_Cmd(TIM3,ENABLE);
}

int16_t Encoder_Get(void)
{
	int16_t Temp;
	Temp = TIM_GetCounter(TIM3);
	TIM_SetCounter(TIM3,0);
	return Temp;//��¼�����CNT
}
