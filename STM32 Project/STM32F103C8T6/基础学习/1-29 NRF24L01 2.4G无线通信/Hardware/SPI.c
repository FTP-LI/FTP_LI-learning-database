#include "stm32f10x.h"                  // Device header

#define CLK GPIO_Pin_5//ʱ����
#define DI GPIO_Pin_7//���������/�ӻ�������MOSI
#define DO GPIO_Pin_6//����������/�ӻ������MISO
#define CS GPIO_Pin_4//ģ��ѡ����CS/SS
#define SPI_PROT GPIOA

//ע�Ͳ���Ϊ���SPI����

void SPI_W_CS(uint8_t BitValue)//�༭CS/SS����
{
	GPIO_WriteBit(SPI_PROT,CS,(BitAction)BitValue);
}

//void SPI_W_SCK(uint8_t BitValue)//�༭SCK����
//{
//	GPIO_WriteBit(SPI_PROT,CLK,(BitAction)BitValue);
//}

//void SPI_W_MOSI(uint8_t BitValue)//�༭DI����
//{
//	GPIO_WriteBit(SPI_PROT,DI,(BitAction)BitValue);
//}

//uint8_t SPI_R_MISO(void)//��ȡDO����
//{
//	return GPIO_ReadInputDataBit(SPI_PROT,DO);
//}

void SPI_InitN(void)//��ʼ��
{
//  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
    
//	GPIO_InitTypeDef GPIO_InitStructure;
// 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//�������ģʽ
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_InitStructure.GPIO_Pin = CLK|DI|CS;
// 	GPIO_Init(SPI_PROT, &GPIO_InitStructure);
        
// 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//��������ģʽ
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_InitStructure.GPIO_Pin = DO;
// 	GPIO_Init(SPI_PROT, &GPIO_InitStructure);

    GPIO_InitTypeDef GPIO_InitStructure;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//�������ģʽ
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_4;//CS/SS
 	GPIO_Init(GPIOA, &GPIO_InitStructure);
    
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;//�������ģʽ
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_5|GPIO_Pin_7;//CLK/DI
 	GPIO_Init(GPIOA, &GPIO_InitStructure);
    
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//��������ģʽ
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;//DO
 	GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    SPI_InitTypeDef SPI_InitStructure;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;//����ģʽѡ����ģʽ
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;//����ģʽѡ��˫��ȫ˫��ģʽ    
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;//����֡ѡ��8λ����֡
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;//��������λ����λ����
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_128;//������Ԥ��Ƶ��ѡ��128��Ƶ
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;//SPIģʽѡ��
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;//NSSģʽѡ�����ģ��NSS
    SPI_InitStructure.SPI_CRCPolynomial = 7;//CRCУ�����ʽ��Ĭ��ֵ7
    SPI_Init(SPI1,&SPI_InitStructure);
    
    SPI_Cmd(SPI1,ENABLE);
    
    SPI_W_CS(1);
    
//	SPI_W_CS(1);
//	SPI_W_SCK(0);//SPIģʽ0
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
//	uint8_t ByteReceive = 0x00,i;
//	
//	for(i=0; i<8; i++)
//	{
//		SPI_W_MOSI(ByteSend & (0x80 >> i));
//		SPI_W_SCK(1);
//		if(SPI_R_MISO() == 1)
//		{
//			ByteReceive |= (0x80 >> i);
//		}
//		SPI_W_SCK(0);
//	}
//	return ByteReceive;
    
    while (SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE) != SET);//�ȴ�TXE
    
    SPI_I2S_SendData(SPI1,ByteSend);
    
    while (SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_RXNE) != SET);//�ȴ�RXNE
        
    return SPI_I2S_ReceiveData(SPI1);
}
