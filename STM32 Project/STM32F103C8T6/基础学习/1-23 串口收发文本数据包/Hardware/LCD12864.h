#ifndef _LCD12864_H_
#define _LCD12864_H_

typedef struct
{
	uint8_t com;//��ʾ��ַ
	uint8_t mun;//��������
	uint8_t *dat;//��ʾ����
}
LCD_InitTypeDef;

/*
��ǰ����ΪGPIOA��8λ����Ϊ���ݽӿ�
GPIOB_0_1_10_11Ϊ��������
*/

#define RS GPIO_Pin_0	//���ݺ�ָ��ʹ�ܣ���Ϊ�������룬��Ϊָ������
#define RW GPIO_Pin_1	//��д���ݣ����������ݿ�֮���GND����Ϊ�����ݣ���Ϊд����
#define EN GPIO_Pin_10	//����ʹ��,Ϊ�߿�ʼ��������
#define PSB GPIO_Pin_11	//������ʹ�ܣ���Ϊ���У���Ϊ����

#define DATA_PROT GPIOA //����������
#define CMD_PROT GPIOB	//����������

#define RS_HIGH GPIO_SetBits(CMD_PROT,RS)//��
#define RS_LOW  GPIO_ResetBits(CMD_PROT,RS)//��

#define RW_HIGH GPIO_SetBits(CMD_PROT,RW)
#define RW_LOW  GPIO_ResetBits(CMD_PROT,RW)

#define EN_HIGH GPIO_SetBits(CMD_PROT,EN)
#define EN_LOW  GPIO_ResetBits(CMD_PROT,EN)

#define PSB_HIGH GPIO_SetBits(CMD_PROT,PSB)
#define PSB_LOW  GPIO_ResetBits(CMD_PROT,PSB)
//��ݿ��ƺ궨�壬��Ӧ�ߵ�״̬

#define CMD 0
#define DATA 1


void LCD12864_Write(uint8_t cmd,uint8_t data);
void LCD12864_Init(void);
void LCD_12864_Open(void);
void LCD_12864_Shut(void);
void LCD12864_Write_Word(LCD_InitTypeDef LCD_InitStructure);


#endif
