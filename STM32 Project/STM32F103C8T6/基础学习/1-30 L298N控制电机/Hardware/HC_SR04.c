#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "HC_SR04.h"

uint32_t HC_Count;//����������붨ʱ����������йأ�Ŀǰ��1MS

void HC_SR04_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//������
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);//��ʱ��
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//�������ģʽ
	GPIO_InitStructure.GPIO_Pin = TRIG;//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//�����ٶ�
	GPIO_Init(HC_SR04_PROT,&GPIO_InitStructure);//��ʼ����������
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;//��������ģʽ
	GPIO_InitStructure.GPIO_Pin = ECHO;//�����ź�
	GPIO_Init(HC_SR04_PROT,&GPIO_InitStructure);//��ʼ����������
	
	TIM_TimeBaseInitTypeDef TIM_InitStructure;
	TIM_InitStructure.TIM_ClockDivision = TIM_CKD_DIV1;//����Ƶ
	TIM_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;//���ϼ���ģʽ
	TIM_InitStructure.TIM_Period = 1000-1;//ARR,���ý��Ϊ1MS��һ���ж�
	TIM_InitStructure.TIM_Prescaler = 72-1;//SPC,Ԥ��Ƶϵ��
	TIM_InitStructure.TIM_RepetitionCounter = DISABLE;//�����Զ���װ
	TIM_TimeBaseInit(TIM2,&TIM_InitStructure);//��ʼ����ʱ��ʱ��
	
	TIM_ClearFlag(TIM2,TIM_FLAG_Update);//��ձ�־λ,�����Ƿ��Ƶʱ�����̴�����־λ
	
	TIM_ITConfig(TIM2,TIM_FLAG_Update|TIM_IT_Trigger,ENABLE);//����������־λ�����ŵ��жϴ���
	TIM_Cmd(TIM2,DISABLE);//�رռ�ʱ��
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);//�жϷ���
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;//�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;//��ռ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;//�����ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//ʹ���ж�
	NVIC_Init(&NVIC_InitStructure);//�жϳ�ʼ��
	
}

void TIM2_IRQHandler(void)//��������ʱ��
{
	if(TIM_GetITStatus(TIM2,TIM_IT_Update) !=0)//��Ϊ0ʱ�����ж�
	{
		TIM_ClearITPendingBit(TIM2,TIM_IT_Update);//����жϱ�־
		HC_Count++;
	}
}

void HC_SR04_Distance(uint16_t *dataA,uint16_t *dataB)
{
	uint32_t Count= 0;
	
	while(GPIO_ReadInputDataBit(HC_SR04_PROT,ECHO) == 1);//��ȡ����״̬,��Ϊ�ߵ�ƽʱ��������ʱHC_SR04���ź�
	GPIO_SetBits(HC_SR04_PROT,TRIG);
	Delay_us(20);//����20΢��ĸߵ�ƽTTL�źţ���������
	GPIO_ResetBits(HC_SR04_PROT,TRIG);//����ź�
	
	while(GPIO_ReadInputDataBit(HC_SR04_PROT,ECHO) == 0);//��ȡ����״̬,��Ϊ�͵�ƽʱ��������ʱHC_SR04���ջ���
	TIM_SetCounter(TIM2,0);//��ʱ����������0
	HC_Count = 0;//��������0
	TIM_Cmd(TIM2,ENABLE);//������ʱ��
	
	while(GPIO_ReadInputDataBit(HC_SR04_PROT,ECHO) == 1);//��ȡ����״̬,��Ϊ�ߵ�ƽʱ��������ʱHC_SR04���ջ������
	TIM_Cmd(TIM2,DISABLE);//�رռ�����
	
	Count = HC_Count * 1000;//ת����΢��us
	Count = Count + TIM_GetCounter(TIM2);//��ʱ�䣬TIM_GetCounter��ȡ��ʱ����CNTֵ����λֵ���1us
	
	*dataA = Count / 58;//��ʽת������				    //���빫ʽ
	*dataB = Count * 0.017;//340/1000000us  340/s/2*100//���ٹ�ʽ
	
}

uint16_t HC_SR04_avange(void)
{
	uint16_t dataA =0, dataB =0; 
	uint32_t dataNA =0,dataNB =0;
	
	HC_SR04_Distance(&dataA,&dataB);
	for(uint8_t i=0; i<5; i++)
		{
			dataNA = dataNA + dataA;//�����ۼ�
			dataNB = dataNB + dataB;
		}
	return (dataNA+dataNB)/5;
}
