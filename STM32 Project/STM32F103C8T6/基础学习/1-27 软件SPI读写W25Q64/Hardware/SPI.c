#include "stm32f10x.h"                  // Device header

#define CLK GPIO_Pin_5//ʱ����
#define DI GPIO_Pin_7//���������/�ӻ�������MOSI
#define DO GPIO_Pin_6//����������/�ӻ������MISO
#define CS GPIO_Pin_4//ģ��ѡ����
#define SPI_PROT GPIOA

void SPI_W_CS(uint8_t BitValue)//�༭CS/SS����
{
	GPIO_WriteBit(SPI_PROT,CS,(BitAction)BitValue);
}

void SPI_W_SCK(uint8_t BitValue)//�༭SCK����
{
	GPIO_WriteBit(SPI_PROT,CLK,(BitAction)BitValue);
}

void SPI_W_MOSI(uint8_t BitValue)//�༭DI����
{
	GPIO_WriteBit(SPI_PROT,DI,(BitAction)BitValue);
}

uint8_t SPI_R_MISO(void)//��ȡDO����
{
	return GPIO_ReadInputDataBit(SPI_PROT,DO);
}

void SPI_InitN(void)//��ʼ��
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//�������ģʽ
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = CLK|DI|CS;
 	GPIO_Init(SPI_PROT, &GPIO_InitStructure);

 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//��������ģʽ
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = DO;
 	GPIO_Init(SPI_PROT, &GPIO_InitStructure);
	
	SPI_W_CS(1);
	SPI_W_SCK(0);//SPIģʽ0
}

void SPI_Start(void)
{
	SPI_W_CS(0);
}

void SPI_Stop(void)
{
	SPI_W_CS(1);
}

uint8_t SPI_SwapByte(uint8_t ByteSend)//����/��дһ���ֽ�����
{
	uint8_t ByteReceive = 0x00,i;
	
	for(i=0; i<8; i++)
	{
		SPI_W_MOSI(ByteSend & (0x80 >> i));
		SPI_W_SCK(1);
		if(SPI_R_MISO() == 1)
		{
			ByteReceive |= (0x80 >> i);
		}
		SPI_W_SCK(0);
	}
	return ByteReceive;
}
