#include "stm32f10x.h"                  // Device header

uint16_t MyDMA_Size;

void MyDMA_Init(uint32_t AddrA, uint32_t AddrB, uint16_t Size)//�����ַ��Ŀ���ַ����������/�������
{
	MyDMA_Size = Size;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
	
	DMA_InitTypeDef DMA_InitStructure;//����DMA
	DMA_InitStructure.DMA_PeripheralBaseAddr = AddrA;//����վ�����ʼ��ַ�����Զ��庯����ȡ
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;//���ݿ��,�ֽڵĴ�С
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Enable;//�Ƿ�����������
	DMA_InitStructure.DMA_MemoryBaseAddr = AddrB;//�洢��վ�����ʼ��ַ�����Զ��庯����ȡ
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;//���ݿ�ȣ��ֽڵĴ�С
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//�Ƿ�����������
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;//���䷽��,����վ����Ϊ����ԴSRC��source��Դͷ
	DMA_InitStructure.DMA_BufferSize = Size;//��������С
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;//�Ƿ��Զ���װ,����ģʽ����������װ
	DMA_InitStructure.DMA_M2M = DMA_M2M_Enable;//ѡ���������������Ӳ��������ʹ���������
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;//���ȼ����е����ȼ�
	DMA_Init(DMA1_Channel1,&DMA_InitStructure);//����DMA1��ͨ��1
	
	DMA_Cmd(DMA1_Channel1, DISABLE);//����DMA1ͨ��1
}
void MyDMA_Transfer(void)
{
	DMA_Cmd(DMA1_Channel1, DISABLE);//ʧ��DMA1ͨ��1
	DMA_SetCurrDataCounter(DMA1_Channel1,MyDMA_Size);//����ת�˼�����
	DMA_Cmd(DMA1_Channel1, ENABLE);//����DMA1ͨ��1
	
	while(DMA_GetFlagStatus(DMA1_FLAG_TC1) == RESET);//�ȴ�ת����ɣ����ת����ɵı�־λ
	DMA_ClearFlag(DMA1_FLAG_TC1);//�����־λ
}
