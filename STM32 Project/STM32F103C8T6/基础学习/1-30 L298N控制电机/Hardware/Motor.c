#include "stm32f10x.h"                  // Device header
#include "PWM.h"

void Motor_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//��ʼ�������������
	
	GPIO_InitTypeDef GPIO_Iitstructure;
	GPIO_Iitstructure.GPIO_Mode = GPIO_Mode_Out_PP;//�������ģʽ
	GPIO_Iitstructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;//����1��2����
	GPIO_Iitstructure.GPIO_Speed = GPIO_Speed_50MHz;//�����ٶ�50MHz
	GPIO_Init(GPIOA,&GPIO_Iitstructure);
	
	PWM_Init();
}

void Motor_SetSpeed(int8_t Speed)
{
	if(Speed >=0 )//��ת
	{
		GPIO_ResetBits(GPIOA,GPIO_Pin_4);//Pin4Ϊ�ߵ�ƽ
		GPIO_SetBits(GPIOA,GPIO_Pin_5);//Pin5Ϊ�͵�ƽ
		PWM_SetCompare1(Speed);
	}
	if(Speed <0 )//��ת
	{
		GPIO_SetBits(GPIOA,GPIO_Pin_4);//Pin4Ϊ�͵�ƽ
		GPIO_ResetBits(GPIOA,GPIO_Pin_5);//Pin5Ϊ�ߵ�ƽ
		PWM_SetCompare1(-Speed);
	}
}
