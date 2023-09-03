#include "stm32f10x.h"                  // Device header
#include "Delay.h"

#define SCL_PROT GPIOB
#define SCL GPIO_Pin_10
#define SDA_PROT GPIOB
#define SDA GPIO_Pin_11
//�����ĸò���������IIC_Init����

void IIC_W_SCL(uint8_t BitValue)//1Ϊ����
{
	GPIO_WriteBit(SCL_PROT,SCL,(BitAction)BitValue);
	Delay_us(10);
}

void IIC_W_SDA(uint8_t BitValue)//1Ϊ����
{
	GPIO_WriteBit(SDA_PROT,SDA,(BitAction)BitValue);
	Delay_us(10);
}

uint8_t IIC_R_SDA(void)
{
	uint8_t BitValue;
	BitValue= GPIO_ReadInputDataBit(SDA_PROT, SDA);
	Delay_us(10);
	return BitValue;
}

void IIC_Init(void)//�ӿڳ�ʼ��
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_InitTypeDef GPIO_Iitstructure;
	GPIO_Iitstructure.GPIO_Mode = GPIO_Mode_Out_OD;//��©���ģʽ
	GPIO_Iitstructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;//����10��11����
	GPIO_Iitstructure.GPIO_Speed = GPIO_Speed_50MHz;//�����ٶ�50MHz
	GPIO_Init(GPIOB,&GPIO_Iitstructure);
	
	GPIO_SetBits(GPIOB,GPIO_Pin_10 |  GPIO_Pin_11);//��ʼ�����ţ�Ĭ��Ϊ�͵�ƽ������Ϊ��
}

void IIC_Start(void)
{
	IIC_W_SDA(1);
	IIC_W_SCL(1);//�ͷ����ţ�SCL��SDA��Ϊ��ʱ���ܻᷢ��������������ͷ�SDA
	IIC_W_SDA(0);//����SDA
	IIC_W_SCL(0);//������SCL
}

void IIC_Stop(void)
{
	IIC_W_SDA(0);//����SDA������ֹ��־
	IIC_W_SCL(1);
	IIC_W_SDA(1);//�ͷ�����
}

void IIC_SendByte(uint8_t Byte)
{
	uint8_t i;
	for(i=0; i<8; i++ )
	{
		IIC_W_SDA(Byte & (0x80 >>i) );//��Byte�����λӳ����SDA��,>>����
		IIC_W_SCL(1);
		IIC_W_SCL(0);//һλ�����ݴ������
	}
}

uint8_t IIC_ReceiveByte(void)
{
	uint8_t i, Byte = 0x00;
	for(i=0; i<8; i++)
	{
		IIC_W_SDA(1);
		IIC_W_SCL(1);
		if(IIC_R_SDA() == 1)
		{
			Byte |= (0x80>>i);
		}
		IIC_W_SCL(0);
	}
	IIC_W_SCL(1);//�ͷ�SCL
	return Byte;
}

void IIC_SendAck(uint8_t AckBit)//дӦ��
{

	IIC_W_SDA(AckBit);
	IIC_W_SCL(1);
	IIC_W_SCL(0);//һλ�����ݴ������
}

uint8_t IIC_ReceiveAck(void)//��Ӧ�� ����ֵΪ0���������
{
	uint8_t  AckBit;
	IIC_W_SDA(1);
	IIC_W_SCL(1);
	AckBit = IIC_R_SDA();
	IIC_W_SCL(0);
	return AckBit;
}
