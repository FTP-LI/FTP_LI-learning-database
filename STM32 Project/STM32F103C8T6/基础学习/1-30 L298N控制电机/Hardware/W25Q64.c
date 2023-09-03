#include "stm32f10x.h"                  // Device header
//#include "SPI.h"
#include "W25Q64_Ins.h"
#include "delay.h"

void W25Q64_Init(void)
{
	SPI_InitN();
}

void W25Q64_ReadID(uint8_t *MID, uint16_t *DID) //MID����ID DID�豸ID
{
	SPI_Start();
	SPI_SwapByte(W25Q64_JEDEC_ID);//����
	*MID = SPI_SwapByte(W25Q64_DUMMY_BYTE);//���ճ���ID
	*DID = SPI_SwapByte(W25Q64_DUMMY_BYTE);//�����豸ID�߰�λ
	*DID <<= 8;//DID����8λ
	*DID |= SPI_SwapByte(W25Q64_DUMMY_BYTE);//�����豸ID�Ͱ�λ
	SPI_Stop();
}

void W25Q64_WriteEnable(void)//дʹ��
{
	SPI_Start();
	SPI_SwapByte(W25Q64_WRITE_ENABLE);
	SPI_Stop();
}

void W25Q64_WaitBusy(void)
{
	uint32_t Timeout;
	SPI_Start();
	SPI_SwapByte(W25Q64_READ_STATUS_REGISTER_1);
	Timeout = 100000;
	while((SPI_SwapByte(W25Q64_DUMMY_BYTE) & 0x01) == 0x01)//�ж�״̬�Ĵ���1�����һλBUSYλ�Ƿ�Ϊ1Ϊ1��ȴ�
	{
		Timeout--;
		if(Timeout == 0)
		{
			break;
		}//��ʱ�˳�
	}
	SPI_Stop();
}

void W25Q64_PageProgram(uint32_t Address, uint8_t *DataArray, uint16_t Count)//д����,����ҳ���
{
	uint16_t i;
	
	W25Q64_WriteEnable();
	
	SPI_Start();
	SPI_SwapByte(W25Q64_PAGE_PROGRAM);
	SPI_SwapByte(Address >> 16);
	SPI_SwapByte(Address >> 8);
	SPI_SwapByte(Address);
	for (i = 0; i < Count; i ++)
	{
		SPI_SwapByte(DataArray[i]);
	}
	SPI_Stop();
	
	W25Q64_WaitBusy();
}

void W25Q64_SectorErase(uint32_t Address)//��������
{
	W25Q64_WriteEnable();
	
	SPI_Start();
	SPI_SwapByte(W25Q64_SECTOR_ERASE_4KB);
	SPI_SwapByte(Address >> 16);
	SPI_SwapByte(Address >> 8);
	SPI_SwapByte(Address);
	SPI_Stop();
	
	W25Q64_WaitBusy();
}

void W25Q64_ReadData(uint32_t Address, uint8_t *DataArray, uint32_t Count)//������
{
	uint32_t i;
	SPI_Start();
	SPI_SwapByte(W25Q64_READ_DATA);
	SPI_SwapByte(Address >> 16);
	SPI_SwapByte(Address >> 8);
	SPI_SwapByte(Address);
	for (i = 0; i < Count; i ++)
	{
		DataArray[i] = SPI_SwapByte(W25Q64_DUMMY_BYTE);
	}
	SPI_Stop();
}
