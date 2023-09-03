#include "stm32f10x.h"                  // Device header

uint16_t AD_Value[4];

void AD_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);//�����ⲿADC1
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//����APIOA
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);//����DMA1
	
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);//ADCCLK=72MHz/6=12MHz
	
	GPIO_InitTypeDef GPIO_Iitstructure;
	GPIO_Iitstructure.GPIO_Mode = GPIO_Mode_AIN;//AINģ������ģʽ
	GPIO_Iitstructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 ;//����0����
	GPIO_Iitstructure.GPIO_Speed = GPIO_Speed_50MHz;//�����ٶ�50MHz
	GPIO_Init(GPIOA,&GPIO_Iitstructure);
	
	ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_55Cycles5);//�趨�����飺ʹ��ADC1������1ͨ������һ����λ�ã�55.5��ADCCLK�Ĳ�������
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 2, ADC_SampleTime_55Cycles5);//�趨�����飺ʹ��ADC1������2ͨ�����ڶ�����λ�ã�55.5��ADCCLK�Ĳ�������
	ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 3, ADC_SampleTime_55Cycles5);//�趨�����飺ʹ��ADC1������3ͨ������������λ�ã�55.5��ADCCLK�Ĳ�������
	ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 4, ADC_SampleTime_55Cycles5);//�趨�����飺ʹ��ADC1������4ͨ������������λ�ã�55.5��ADCCLK�Ĳ�������

	ADC_InitTypeDef ADC_IintStructure;
	ADC_IintStructure.ADC_Mode = ADC_Mode_Independent;//ADC����ģʽ
	ADC_IintStructure.ADC_DataAlign = ADC_DataAlign_Right;//ADC�����Ҷ���
	ADC_IintStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//ADC�ⲿ����Դ����ʹ��
	ADC_IintStructure.ADC_ContinuousConvMode = ENABLE;//�Ƿ�����ת��,��
	ADC_IintStructure.ADC_ScanConvMode = ENABLE;//�Ƿ����ɨ��,��ͨ��ɨ�裬��ͨ����ɨ��
	ADC_IintStructure.ADC_NbrOfChannel = 4;//ͨ����Ŀ
	ADC_Init(ADC1,&ADC_IintStructure);
	
	DMA_InitTypeDef DMA_InitStructure;//����DMA
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;//����վ�����ʼ��ַ�����Զ��庯����ȡ��ADC DR��ʱ�洢ģ��ĵ�ַ
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;//���ݿ��,���ִ�С16λ
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//�Ƿ�����������������һ������Դ
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&AD_Value;//�洢��վ�����ʼ��ַ�����Զ��庯����ȡ
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;//���ݿ�ȣ����ֵĴ�С
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//�Ƿ���������������洢��ַ
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;//���䷽��,����վ����Ϊ����ԴSRC��source��Դͷ
	DMA_InitStructure.DMA_BufferSize = 4;//����������������Ҫ��������ݸ�����
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;//�Ƿ��Զ���װ,����ģʽ�������Զ���װ
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;//ѡ���������������Ӳ��������ʹ��Ӳ������
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;//���ȼ����е����ȼ�
	DMA_Init(DMA1_Channel1,&DMA_InitStructure);//����DMA1��ͨ��1��ӦΪADC1��Ӳ������ͨ��ֻ�ӵ�DMA1��ͨ��1
	
	DMA_Cmd(DMA1_Channel1, ENABLE);//����DMA1ͨ��1
	
	ADC_DMACmd(ADC1,ENABLE);//����ADC����DMA��Ӳ���ź�
	
	ADC_Cmd(ADC1,ENABLE);//����ADC1
	
	ADC_ResetCalibration(ADC1);//������λУ׼
	while(ADC_GetResetCalibrationStatus(ADC1) == SET);//�ȴ���λУ׼���,У׼��ɺ���ֵΪ0���˳�ѭ��
	ADC_StartCalibration(ADC1);//��ʼУ׼
	while(ADC_GetCalibrationStatus(ADC1) == SET);//�ȴ�У׼���
	
	ADC_SoftwareStartConvCmd(ADC1,ENABLE);//�������ת��

}
/*
void AD_GetValue(void)
{
	DMA_Cmd(DMA1_Channel1, DISABLE);//ʧ��DMA1ͨ��1
	DMA_SetCurrDataCounter(DMA1_Channel1,4);//����ת�˼�����,ת��4��
	DMA_Cmd(DMA1_Channel1, ENABLE);//����DMA1ͨ��1
	
	while(DMA_GetFlagStatus(DMA1_FLAG_TC1) == RESET);//�ȴ�ת����ɣ����ת����ɵı�־λ
	DMA_ClearFlag(DMA1_FLAG_TC1);//�����־λ
	
}
*/
//��ȡADC1ת¼ֵ�����������ã�����ر�ADC����ת����DMA���Զ���װ�������������е��ý���ת������
