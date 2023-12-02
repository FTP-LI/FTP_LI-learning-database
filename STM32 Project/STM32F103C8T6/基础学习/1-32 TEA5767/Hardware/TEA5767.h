#ifndef _TEA5767_H_
#define _TEA5767_H_

/*��������������������������������λ��������������������*/
#define TEA5767_ADDR_W      0xc0    // TEA5767 д��ַ
#define TEA5767_ADDR_R      0xc1    // TEA5767 ����ַ

#define TEA5767_MAX_KHZ     108000  // ���Ƶ�� 108M
#define TEA5767_MIN_KHZ     87800   // ���Ƶ�� 87.5M

#define TEA5767_MUTE_ON     1       // �Ǿ���
#define TEA5767_MUTE_OFF    0       // ����

#define TEA5767_SEARCH_UP   1       // ��������
#define TEA5767_SEARCH_DOWN 0       // ��������

/*������������������������ȫ�ֱ�����������������������*/

extern unsigned long g_frequency;//Ƶ��

/*����������������������������API��������������������*/

void TEA5767_Init(void);
void TEA5767_Write(void);
void TEA5767_Read(void);
void TEA5767_GetPLL(void);
uint32_t TEA5767_GetFrequency(void);
void TEA5767_SetFrequency(uint32_t frequency);
void TEA5767_Search(uint8_t mode);
void TEA5767_AutoSearch(uint8_t mode);
void TEA5767_Mute(uint8_t mode);

#endif
