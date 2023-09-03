#include "stm32f10x.h"                  // Device header
#include "IIC.h"
#include "MPU6050_Reg.h"

#define MUP6050_ADDRESS 0xD0


void MPU6050_WriteReg(uint8_t RegAddress, uint8_t Data)//RegAddressģ��ļĴ�����ַ��Data��Ӧ����
{
	IIC_Start();
	IIC_SendByte(MUP6050_ADDRESS);
	IIC_ReceiveAck();//����Ӧ��λ
	IIC_SendByte(RegAddress);
	IIC_ReceiveAck();
	IIC_SendByte(Data);
	IIC_ReceiveAck();//��һ�ֽ�
	IIC_Stop();
}

uint8_t MPU6050_ReadReg(uint8_t RegAddress)
{
	uint8_t Data;
	IIC_Start();
	IIC_SendByte(MUP6050_ADDRESS);
	IIC_ReceiveAck();//����Ӧ��λ
	IIC_SendByte(RegAddress);
	IIC_ReceiveAck();//��λλ��
	
	IIC_Start();
	IIC_SendByte(MUP6050_ADDRESS | 0x01);//��ȡ����
	IIC_ReceiveAck();
	Data = IIC_ReceiveByte();
	IIC_SendAck(0);//��������Ӧ��
	IIC_Stop();
	
	return Data;
}



void MPU6050_Init(void)
{
	IIC_Init();
	MPU6050_WriteReg(MPU6050_PWR_MGMT_1,0x01);//ʹ��MPU6050��������ʱ��
	MPU6050_WriteReg(MPU6050_PWR_MGMT_2,0x00);//���᲻��Ҫ����Ƶ��
	MPU6050_WriteReg(MPU6050_SMPLRT_DIV,0x09);//10��Ƶ
	MPU6050_WriteReg(MPU6050_CONFIG,0x06);//ƽ���˲�
	MPU6050_WriteReg(MPU6050_GYRO_CONFIG,0x18);//�����ǲ��Բ⣬�������
	MPU6050_WriteReg(MPU6050_ACCEL_CONFIG,0x18);//���ٶȼƲ��Բ⣬�������
}

void MPU6050_GetData(int16_t *AccX, int16_t *AccY,int16_t *AccZ,
					int16_t *GyroX, int16_t *GyroY,int16_t *GyroZ)
{
	uint8_t DataH, DataL;
	
	DataH = MPU6050_ReadReg(MPU6050_ACCEL_XOUT_H);
	DataL = MPU6050_ReadReg(MPU6050_ACCEL_XOUT_L);
	*AccX = (DataH << 8) | DataL; //��ȡ16λ������
	
	DataH = MPU6050_ReadReg(MPU6050_ACCEL_YOUT_H);
	DataL = MPU6050_ReadReg(MPU6050_ACCEL_YOUT_L);
	*AccY = (DataH << 8) | DataL; //��ȡ16λ������

	DataH = MPU6050_ReadReg(MPU6050_ACCEL_ZOUT_H);
	DataL = MPU6050_ReadReg(MPU6050_ACCEL_ZOUT_L);
	*AccZ = (DataH << 8) | DataL; //��ȡ16λ������
	
	DataH = MPU6050_ReadReg(MPU6050_GYRO_XOUT_H);
	DataL = MPU6050_ReadReg(MPU6050_GYRO_XOUT_L);
	*GyroX = (DataH << 8) | DataL; //��ȡ16λ������

	DataH = MPU6050_ReadReg(MPU6050_GYRO_YOUT_H);
	DataL = MPU6050_ReadReg(MPU6050_GYRO_YOUT_L);
	*GyroY = (DataH << 8) | DataL; //��ȡ16λ������
	
	DataH = MPU6050_ReadReg(MPU6050_GYRO_ZOUT_H);
	DataL = MPU6050_ReadReg(MPU6050_GYRO_ZOUT_L);
	*GyroZ = (DataH << 8) | DataL; //��ȡ16λ������
	
}
