#include "stm32f10x.h"                  // Device header

void AD_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);//�����ⲿADC1
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//����APIOA
	
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);//ADCCLK=72MHz/6=12MHz
	
	GPIO_InitTypeDef GPIO_Iitstructure;
	GPIO_Iitstructure.GPIO_Mode = GPIO_Mode_AIN;//AINģ������ģʽ
	GPIO_Iitstructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 ;//����0����
	GPIO_Iitstructure.GPIO_Speed = GPIO_Speed_50MHz;//�����ٶ�50MHz
	GPIO_Init(GPIOA,&GPIO_Iitstructure);
	
	
	ADC_InitTypeDef ADC_IintStructure;
	ADC_IintStructure.ADC_Mode = ADC_Mode_Independent;//ADC����ģʽ
	ADC_IintStructure.ADC_DataAlign = ADC_DataAlign_Right;//ADC�����Ҷ���
	ADC_IintStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//ADC�ⲿ����Դ����ʹ��
	ADC_IintStructure.ADC_ContinuousConvMode = DISABLE;//�Ƿ�����ת��
	ADC_IintStructure.ADC_ScanConvMode = DISABLE;//�Ƿ����ɨ��,��ͨ��ɨ�裬��ͨ����ɨ��
	ADC_IintStructure.ADC_NbrOfChannel = 1;//ͨ����Ŀ
	ADC_Init(ADC1,&ADC_IintStructure);
	
	ADC_Cmd(ADC1,ENABLE);//����ADC1
	ADC_ResetCalibration(ADC1);//������λУ׼
	while(ADC_GetResetCalibrationStatus(ADC1) == SET);//�ȴ���λУ׼���,У׼��ɺ���ֵΪ0���˳�ѭ��
	ADC_StartCalibration(ADC1);//��ʼУ׼
	while(ADC_GetCalibrationStatus(ADC1) == SET);//�ȴ�У׼���
	
}

uint16_t AD_GetValue(uint8_t ADC_Channel)
{
	ADC_RegularChannelConfig(ADC1, ADC_Channel, 1, ADC_SampleTime_55Cycles5);//�趨�����飺ʹ��ADC1������xͨ������һ����λ�ã�55.5��ADCCLK�Ĳ�������
	ADC_SoftwareStartConvCmd(ADC1,ENABLE);//�������ת��
	while(ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC) == RESET);//�ȴ�ת�����
	return ADC_GetConversionValue(ADC1);//��ȡת��ֵ
}
