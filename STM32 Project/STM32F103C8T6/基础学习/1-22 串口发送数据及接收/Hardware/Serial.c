#include "stm32f10x.h"// Device header
#include <stdio.h>
#include <stdarg.h>

uint8_t Serial_RxData;
uint8_t Serial_RxFlag;

void Serial_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
    GPIO_InitTypeDef GPIO_Iitstructure;
	GPIO_Iitstructure.GPIO_Mode = GPIO_Mode_AF_PP;//�������ģʽ
	GPIO_Iitstructure.GPIO_Pin = GPIO_Pin_9 ;//����9����
	GPIO_Iitstructure.GPIO_Speed = GPIO_Speed_50MHz;//�����ٶ�50MHz
	GPIO_Init(GPIOA,&GPIO_Iitstructure);//����GPIOA	9
	
	GPIO_Iitstructure.GPIO_Mode = GPIO_Mode_IPU;//��������ģʽ
	GPIO_Iitstructure.GPIO_Pin = GPIO_Pin_10 ;//����10����
	GPIO_Iitstructure.GPIO_Speed = GPIO_Speed_50MHz;//�����ٶ�50MHz
	GPIO_Init(GPIOA,&GPIO_Iitstructure);//����GPIOA	10

	
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = 9600;//���ò�����
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//����������������ʹ��������
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;//TX�������ģʽ������ģʽ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����У��ģʽ����У��
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//����ֹͣλ��1λֹͣλ
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�����ֳ���8λ�ֳ�
	USART_Init(USART1,&USART_InitStructure);//����USART1
	
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);//����RXNE���ж�
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����жϵ����ȼ�
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//��ʹ��ͨ���Ƿ����б���
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;//��ռ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;//�����ȼ�
	NVIC_Init(&NVIC_InitStructure);//�����жϲ���
	
	
	USART_Cmd(USART1,ENABLE);//����USART1
}

void Serial_SendByte(uint16_t Byte)//���͵��ֽ�
{
	USART_SendData(USART1,Byte);
	while (USART_GetFlagStatus(USART1,USART_FLAG_TXE) == RESET );//��鴫�����ݼĴ����ձ�־λ�������ݴ�����ϻ���1
}

void Serial_SendArray(uint16_t *Array,uint16_t Length)//������������
{
	uint16_t i;
	for(i=0;i<Length;i++)
	{
		Serial_SendByte(Array[i]);
	}
}

void Serial_SendString(char *String)//�����ַ���
{
	uint8_t i;
	for(i=0;String[i]!='\0';i++)
	{
		Serial_SendByte(String[i]);
	}
}

uint32_t Serial_Pow(uint32_t X,uint32_t Y)//�η��������ж�����λ��
{
	uint32_t Result = 1;
	while(Y --)
	{
		Result *= X;
	}
	return Result;
}

void Serial_SendNumber(uint32_t Number,uint8_t Lengeh)
{
	uint8_t i;
	for(i=0 ;i<Lengeh ;i++)
	{
		Serial_SendByte(Number/Serial_Pow(10,Lengeh-i-1)%10+'0');
	}
}

int fputc(int ch, FILE *f)
{
	Serial_SendByte(ch);
	return ch;
}

void Serial_Printf(char *format, ...)
{
	char String[100];
	va_list arg;
	va_start ( arg, format);
	vsprintf( String, format, arg);
	va_end(arg);
	Serial_SendString(String);
}

uint8_t Serial_GetRxFlag(void)//��װ��־λ����
{
	if(Serial_RxFlag == 1)
	{
		Serial_RxFlag = 0;
		return 1;
	}
	return 0;
}

uint8_t Serial_GetRxData(void)//��װ���ݻ�ȡ����
{
	return Serial_RxData;
}

void USART1_IRQHandler(void)//�ж�ת������
{
		if(USART_GetFlagStatus(USART1,USART_FLAG_RXNE)==SET)
		{
			Serial_RxData=USART_ReceiveData(USART1);
			Serial_RxFlag = 1;
			USART_ClearITPendingBit(USART1,USART_FLAG_RXNE);//�����־λ
		}
}
