#include "stm32f10x.h"                  // Device header
#include "delay.h"
#include "OLED.h"
#include "Timer.h"

int16_t Num;//16λȫ�ֱ���

int main(void)
{
	OLED_Init();
	Timer_Init();
	
	OLED_ShowString(1,1,"Num:");
	
	while(1)
	{
		OLED_ShowNum(1,5,Num,5);
		OLED_ShowNum(2,5,TIM_GetCounter (TIM2),5);//��ʾ�Զ���װֵ
	}
}

void TIM2_IRQHandler(void)//TIM2�жϺ���
{
	if(TIM_GetITStatus(TIM2,TIM_IT_Update)==SET)//��ȡTIM2��ʱ�Ӹ��±�׼λ
	{
		Num++;
		TIM_ClearITPendingBit(TIM2,TIM_IT_Update);//���TIM2��ʱ�Ӹ��±�׼λ
	}
}	
