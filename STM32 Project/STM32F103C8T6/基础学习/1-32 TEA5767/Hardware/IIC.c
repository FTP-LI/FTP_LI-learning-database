#include "stm32f10x.h"                  // Device header
#include "Delay.h"
//����������Ϊ���ģ��IICЭ�飬��Delay����֧��
#define SCL_PROT GPIOB
#define IIC_SCL_PIN GPIO_Pin_0

#define SDA_PROT GPIOB
#define IIC_SDA_PIN GPIO_Pin_1

#define IIC_SDA_READ()      GPIO_ReadInputDataBit(IIC_SDA_PORT, IIC_SDA_PIN) 

//�����ĸò���������IIC_Init����

void IIC_W_SCL(uint8_t BitValue)//1Ϊ����
{
	GPIO_WriteBit(SCL_PROT,IIC_SCL_PIN,(BitAction)BitValue);
	Delay_us(10);
}

void IIC_W_SDA(uint8_t BitValue)//1Ϊ����
{
	GPIO_WriteBit(SDA_PROT,IIC_SDA_PIN,(BitAction)BitValue);
	Delay_us(10);
}

uint8_t IIC_R_SDA(void)
{
	uint8_t BitValue;
	BitValue= GPIO_ReadInputDataBit(SDA_PROT, IIC_SDA_PIN);
	Delay_us(10);
	return BitValue;
}

void IIC_Init(void)//�ӿڳ�ʼ��
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_InitTypeDef GPIO_Iitstructure;
    GPIO_Iitstructure.GPIO_Speed = GPIO_Speed_50MHz;//�����ٶ�50MHz
	GPIO_Iitstructure.GPIO_Mode = GPIO_Mode_Out_OD;//��©���ģʽ
	GPIO_Iitstructure.GPIO_Pin = IIC_SCL_PIN;
	GPIO_Init(SCL_PROT,&GPIO_Iitstructure);
    GPIO_Iitstructure.GPIO_Pin =IIC_SDA_PIN;
    GPIO_Init(SDA_PROT,&GPIO_Iitstructure);
	
	IIC_W_SCL(0);
    IIC_W_SDA(0);//��ʼ������
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

void IIC_SendAck(uint8_t AckBit)//дӦ�� д0�򷢳��ź�
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

uint8_t IIC_WaitAck(void)
{
    uint8_t result = 0;
    if(IIC_ReceiveAck())
    {
        result = 1;
    }
    else 
    {
        result = 0;
    }
    return result;
}

void IIC_Ack(void)
{
    IIC_SendAck(0);
}
void IIC_NO_Ack(void)
{  
    IIC_SendAck(1);
}
