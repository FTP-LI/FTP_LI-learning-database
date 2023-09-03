#include "stm32f10x.h"                  // Device header
#include "MPU6050_Reg.h"

#define MUP6050_ADDRESS 0xD0

void MPU6050_WaitEvent(I2C_TypeDef* I2Cx, uint32_t I2C_EVENT)//��װ��⺯�������ӳ�ʱ��ⲿ��
{
	uint32_t TimeOut;
	TimeOut = 10000;
	while(I2C_CheckEvent(I2Cx,I2C_EVENT)!= SUCCESS)//��ʱ��ⲿ��
	{
		TimeOut--;
		if(TimeOut == 0)
		{
			break;
		}
	}
}

void MPU6050_WriteReg(uint8_t RegAddress, uint8_t Data)//RegAddressģ��ļĴ�����ַ��Data��Ӧ����
{
	I2C_GenerateSTART(I2C2, ENABLE);
	MPU6050_WaitEvent(I2C2,I2C_EVENT_MASTER_MODE_SELECT);//���EV5(����ģʽѡ��)�¼�
	I2C_Send7bitAddress(I2C2,MUP6050_ADDRESS,I2C_Direction_Transmitter);
	MPU6050_WaitEvent(I2C2,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);//���EV6(��������ñ�־λ)�¼�
	I2C_SendData(I2C2,RegAddress);
	MPU6050_WaitEvent(I2C2,I2C_EVENT_MASTER_BYTE_TRANSMITTING);//���EV8(�ֽ����ڷ���)�¼�
	I2C_SendData(I2C2,Data);
	MPU6050_WaitEvent(I2C2,I2C_EVENT_MASTER_BYTE_TRANSMITTED);//���EV8_2(�ֽڷ������)�¼�
	I2C_GenerateSTOP(I2C2, ENABLE);
}

uint8_t MPU6050_ReadReg(uint8_t RegAddress)
{
	uint8_t Data;
	I2C_GenerateSTART(I2C2, ENABLE);
	MPU6050_WaitEvent(I2C2,I2C_EVENT_MASTER_MODE_SELECT);//���EV5(����ģʽѡ��)�¼�	
	I2C_Send7bitAddress(I2C2,MUP6050_ADDRESS,I2C_Direction_Transmitter);
	MPU6050_WaitEvent(I2C2,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);//���EV6(��������ñ�־λ)�¼�
	I2C_SendData(I2C2,RegAddress);
	MPU6050_WaitEvent(I2C2,I2C_EVENT_MASTER_BYTE_TRANSMITTED);//���EV8(�ֽ����ڷ���)�¼�
	
	I2C_GenerateSTART(I2C2, ENABLE);
	MPU6050_WaitEvent(I2C2,I2C_EVENT_MASTER_MODE_SELECT);//���EV5(����ģʽѡ��)�¼�
	I2C_Send7bitAddress(I2C2 ,MUP6050_ADDRESS, I2C_Direction_Receiver);
	MPU6050_WaitEvent(I2C2,I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED);//���EV6(��������ñ�־λ)�¼�
	I2C_AcknowledgeConfig(I2C2, DISABLE);
	I2C_GenerateSTOP(I2C2,ENABLE);
	MPU6050_WaitEvent(I2C2,I2C_EVENT_MASTER_BYTE_RECEIVED);//���EV7(��������ñ�־λ)�¼�
	Data = I2C_ReceiveData(I2C2);
	
	I2C_AcknowledgeConfig(I2C2, ENABLE);//��������Ӧ��
	
	return Data;
}



void MPU6050_Init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_InitTypeDef GPIO_Iitstructure;
	GPIO_Iitstructure.GPIO_Mode = GPIO_Mode_AF_OD;//���ÿ�©ģʽ
	GPIO_Iitstructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;//����1��2����
	GPIO_Iitstructure.GPIO_Speed = GPIO_Speed_50MHz;//�����ٶ�50MHz
	GPIO_Init(GPIOB,&GPIO_Iitstructure);
	
	I2C_InitTypeDef I2C_InitStructure;
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;//I2Cģʽ
	I2C_InitStructure.I2C_ClockSpeed = 50000;//SCLʱ��Ƶ�ʣ������ٶ����400KHz��������50KHz
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;//SCLռ�ձȣ��ڸ���ģʽ����Ҫ�������ڱ�׼�ٶ��£�100KHz�ڣ������ޱ仯	
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;//Ӧ��λ����
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;//�ӻ�ģʽ�µ���Ӧ��ַ
	I2C_InitStructure.I2C_OwnAddress1 = 0x00;//IC2�ӻ�ģʽ�µ���Ӧ��ַ
	I2C_Init(I2C2,&I2C_InitStructure);
	
	I2C_Cmd(I2C2, ENABLE);
	
	MPU6050_WriteReg(MPU6050_PWR_MGMT_1,0x01);//ʹ��MPU6050��������ʱ��
	MPU6050_WriteReg(MPU6050_PWR_MGMT_2,0x00);//���᲻��Ҫ����Ƶ��
	MPU6050_WriteReg(MPU6050_SMPLRT_DIV,0x09);//10��Ƶ
	MPU6050_WriteReg(MPU6050_CONFIG,0x06);//ƽ���˲�
	MPU6050_WriteReg(MPU6050_GYRO_CONFIG,0x18);//�����ǲ��Բ⣬�������
	MPU6050_WriteReg(MPU6050_ACCEL_CONFIG,0x18);//���ٶȼƲ��Բ⣬�������
}

void MPU6050_GetData(int16_t *AccX, int16_t *AccY,int16_t *GyroZ)
//    ,int16_t *AccZ,int16_t *GyroX, int16_t *GyroY,int16_t *GyroZ)
{
	uint8_t DataH, DataL;
	
	DataH = MPU6050_ReadReg(MPU6050_ACCEL_XOUT_H);
	DataL = MPU6050_ReadReg(MPU6050_ACCEL_XOUT_L);
	*AccX = (DataH << 8) | DataL; //��ȡ16λ������
	
	DataH = MPU6050_ReadReg(MPU6050_ACCEL_YOUT_H);
	DataL = MPU6050_ReadReg(MPU6050_ACCEL_YOUT_L);
	*AccY = (DataH << 8) | DataL; //��ȡ16λ������

//	DataH = MPU6050_ReadReg(MPU6050_ACCEL_ZOUT_H);
//	DataL = MPU6050_ReadReg(MPU6050_ACCEL_ZOUT_L);
//	*AccZ = (DataH << 8) | DataL; //��ȡ16λ������
//	
//	DataH = MPU6050_ReadReg(MPU6050_GYRO_XOUT_H);
//	DataL = MPU6050_ReadReg(MPU6050_GYRO_XOUT_L);
//	*GyroX = (DataH << 8) | DataL; //��ȡ16λ������

//	DataH = MPU6050_ReadReg(MPU6050_GYRO_YOUT_H);
//	DataL = MPU6050_ReadReg(MPU6050_GYRO_YOUT_L);
//	*GyroY = (DataH << 8) | DataL; //��ȡ16λ������
	
	DataH = MPU6050_ReadReg(MPU6050_GYRO_ZOUT_H);
	DataL = MPU6050_ReadReg(MPU6050_GYRO_ZOUT_L);
	*GyroZ = (DataH << 8) | DataL; //��ȡ16λ������
	
}
