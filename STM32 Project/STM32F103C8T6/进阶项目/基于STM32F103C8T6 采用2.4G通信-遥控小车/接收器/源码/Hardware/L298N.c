#include "stm32f10x.h"                  // Device header
#include "PWM.h"

#define CH1 GPIO_Pin_8
#define CH2 GPIO_Pin_9
#define CH3 GPIO_Pin_10
#define CH4 GPIO_Pin_11
#define L298N_PROTL GPIOA//ǰ����
#define CH5 GPIO_Pin_12
#define CH6 GPIO_Pin_13
#define CH7 GPIO_Pin_14
#define CH8 GPIO_Pin_15
#define L298N_PROTR GPIOB//������

#define CH1_L GPIO_ResetBits(L298N_PROTL,CH1)
#define CH1_H GPIO_SetBits(L298N_PROTL,CH1)

#define CH2_L GPIO_ResetBits(L298N_PROTL,CH2)
#define CH2_H GPIO_SetBits(L298N_PROTL,CH2)

#define CH3_L GPIO_ResetBits(L298N_PROTL,CH3)
#define CH3_H GPIO_SetBits(L298N_PROTL,CH3)

#define CH4_L GPIO_ResetBits(L298N_PROTL,CH4)
#define CH4_H GPIO_SetBits(L298N_PROTL,CH4)

#define CH5_L GPIO_ResetBits(L298N_PROTR,CH5)
#define CH5_H GPIO_SetBits(L298N_PROTR,CH5)

#define CH6_L GPIO_ResetBits(L298N_PROTR,CH6)
#define CH6_H GPIO_SetBits(L298N_PROTR,CH6)

#define CH7_L GPIO_ResetBits(L298N_PROTR,CH7)
#define CH7_H GPIO_SetBits(L298N_PROTR,CH7)

#define CH8_L GPIO_ResetBits(L298N_PROTR,CH8)
#define CH8_H GPIO_SetBits(L298N_PROTR,CH8)

void L298N_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = CH1|CH2|CH3|CH4;
 	GPIO_Init(L298N_PROTL, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = CH5|CH6|CH7|CH8;
 	GPIO_Init(L298N_PROTR, &GPIO_InitStructure);
    PWM_Init();
}

void L_Q(uint8_t dir)//��ǰ�ֿ��ƣ�0��ת��1��ת,2ͣת
{
    if(dir == 0)
    {
       CH1_L;
       CH2_H;//��ǰ        
    }
    else if(dir == 1)
    {
       CH1_H;
       CH2_L;//��ǰ        
    }
    else if(dir == 2)
    {
       CH1_H;
       CH2_H;//��ǰ        
    }
}

void L_H(uint8_t dir)//����ֿ���
{
    if(dir == 0)
    {
       CH7_H;
       CH8_L;//���        
    }
    else if(dir == 1)
    {
       CH7_L;
       CH8_H;//���        
    }
    else if(dir == 2)
    {
       CH7_H;
       CH8_H;//��ǰ        
    }    
}

void R_Q(uint8_t dir)//��ǰ�ֿ���
{
    if(dir == 0)
    {
       CH3_L;
       CH4_H;//��ǰ        
    }
    else if(dir == 1)
    {
       CH3_H;
       CH4_L;//��ǰ        
    }
    else if(dir == 2)
    {
       CH3_H;
       CH4_H;//��ǰ        
    }
}

void R_H(uint8_t dir)//�Һ��ֿ���
{
    if(dir == 0)
    {
       CH5_H;
       CH6_L;//�Һ�        
    }
    else if(dir == 1)
    {
       CH5_L;
       CH6_H;//�Һ�       
    }
    else if(dir == 2)
    {
       CH5_H;
       CH5_H;//��ǰ        
    }    
}

void L298N_Function(uint8_t dir)
{
    if(dir == 0)//ǰ��
    {
        L_Q(0);
        L_H(0);
        R_Q(0);
        R_H(0);
    }
    else if(dir == 1)//����
    {
        L_Q(1);
        L_H(1);
        R_Q(1);
        R_H(1);   
    }
    else if(dir == 2)//��ƽ��
    {
        L_Q(0);
        L_H(1);
        R_Q(1);
        R_H(0);   
    }
    else if(dir == 3)//��ƽ��
    {
        L_Q(1);
        L_H(0);
        R_Q(0);
        R_H(1);
    }
    else if(dir == 4)//����ת
    {
        L_Q(1);
        L_H(1);
        R_Q(0);
        R_H(0);
    }
    else if(dir == 5)//����ת
    {
        L_Q(0);
        L_H(0);
        R_Q(1);
        R_H(1);
    }
    
}
void L298N_Speed(uint8_t Speed)
{
    PWM_SetCompare1(Speed);
    PWM_SetCompare2(Speed); 
    PWM_SetCompare3(Speed); 
    PWM_SetCompare4(Speed);     
}

void L298N_STOP(void)
{
    CH1_H;
    CH2_H;
    CH3_H;
    CH4_H;
    CH5_H;
    CH6_H;
    CH7_H;
    CH8_H;    
    
}
