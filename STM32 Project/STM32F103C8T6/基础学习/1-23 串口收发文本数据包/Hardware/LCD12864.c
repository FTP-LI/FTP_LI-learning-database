#include "stm32f10x.h"                  // Device header
#include "LCD12864.h"
#include "delay.h"

void LCD12864_Write(uint8_t cmd,uint8_t data)//����ָ������� cmd 1 д���� 0 дָ��
{
	if(cmd == 1)//�жϵ�ǰ�����ݻ���ָ��
	{
		RS_HIGH;
	}
	else RS_LOW;
	RW_LOW;
	EN_LOW;//����׼��
	GPIO_Write(DATA_PROT,data);
	Delay_ms(2);//��֤��������
	EN_HIGH;//��������
	Delay_ms(2);//��֤���ݴ���
	EN_LOW;
}

void LCD12864_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_InitTypeDef GPIO_Iitstructure;
	GPIO_Iitstructure.GPIO_Mode = GPIO_Mode_Out_PP;//�������ģʽ
	GPIO_Iitstructure.GPIO_Pin = 0x00FF ;//���õ�8λ���ţ�0000 0000 1111 1111��
	GPIO_Iitstructure.GPIO_Speed = GPIO_Speed_10MHz;//�����ٶ�10MHz
	GPIO_Init(DATA_PROT,&GPIO_Iitstructure);//����GPIOA	
	
	GPIO_Iitstructure.GPIO_Mode = GPIO_Mode_Out_PP;//�������ģʽ
	GPIO_Iitstructure.GPIO_Pin = RS|RW|EN|PSB ;//����
	GPIO_Iitstructure.GPIO_Speed = GPIO_Speed_10MHz;//�����ٶ�10MHz
	GPIO_Init(CMD_PROT,&GPIO_Iitstructure);//����GPIOB	
	//���ų�ʼ��
	
	PSB_HIGH;
	LCD12864_Write(CMD,0x30);//�������� 001<-1 00<-00(����ָ�8λ���ݽӿ�)
	LCD12864_Write(CMD,0x06);//0000 1<-1<-00  AC���� �����ƶ�
	LCD12864_Write(CMD,0x0C);//������ʾ����ʾ���
	LCD12864_Write(CMD,0x01);//����
	LCD12864_Write(CMD,0x80);//ȷ����ʼλ��Ϊһ��һ��
	//��ʼ��ģ��
}

void LCD_12864_Open(void)
{
	LCD12864_Write(CMD,0x0C);//������ʾ����ʾ��꼰��˸
}

void LCD_12864_Shut(void)
{
	LCD12864_Write(CMD,0x08);//�ر���ʾ����ʾ���
}

void LCD12864_Write_Word(LCD_InitTypeDef LCD_InitStructure)
{
	LCD12864_Write( CMD, LCD_InitStructure.com);
	
	for(uint8_t i=0;i<LCD_InitStructure.mun;i++)
	{
		LCD12864_Write(DATA,*LCD_InitStructure.dat++);		
	}
}
