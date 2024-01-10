#include <STC15F2K60S2.H>
#include "intrins.h"
#include "E2ROM.h"

#define CMD_IDLE 0                  //����ģʽ
#define CMD_READ 1                  //IAP�ֽڶ�����
#define CMD_PROGRAM 2               //IAP�ֽڱ������
#define CMD_ERASE 3                 //IAP������������
#define ENABLE_IAP 0x82             //ifSYSCLK<12MHZ
#define IAP_ADDRESS 0x0000
#define Part_1 0x0000
#define Part_2 0x0200
#define Part_3 0x600

//����������ַ 0x0000 0x0200 0x600 �����о�����



void IapIdle(void)//�ر�IAP

{
       IAP_CONTR= 0;                            //�ر�IAP����
       IAP_CMD= 0;                              //���ָ�����
       IAP_TRIG= 0;                             //��մ������Ĵ���
       IAP_ADDRH= 0x80;                         //����ַ���õ���IAP����
       IAP_ADDRL= 0;
}

uint8_t IapReadByte(uint16_t addr)//��ISP/IAP/EEPROM�����ȡһ���ֽ�

{
       uint8_t dat;                             //���ݻ�����
       IAP_CONTR= ENABLE_IAP;                   //ʹ��IAP
       IAP_CMD= CMD_READ;                       //д�������0x5a��
       IAP_TRIG= 0xa5;                          //���ö�ȡ����
       IAP_ADDRL= addr;                         //����IAP�Ͱ�λ��ַ
       IAP_ADDRH= addr >> 8;                    //����IAP�߰�λ��ַ  
       IAP_TRIG= 0x5a;                          //д�������0xa5��
       _nop_();                                 //�ȴ�ISP/IAP/EEPROM�������   
       dat= IAP_DATA;                           //��ISP/IAP/EEPROM����
       IapIdle();                               //�ر�IAP����
       return dat;                              //����

}

void IapProgramByte(uint16_t addr, uint8_t dat)//дһ���ֽڵ�ISP/IAP/EEPROM����

{

       IAP_CONTR= ENABLE_IAP;
       IAP_CMD= CMD_PROGRAM;
       IAP_ADDRL= addr;
       IAP_ADDRH= addr >> 8;
       IAP_DATA= dat;
       IAP_TRIG= 0x5a;
       IAP_TRIG= 0xa5;
       _nop_();
       IapIdle();
}

void IapEraseSector(uint16_t addr)//��������

{
       IAP_CONTR= ENABLE_IAP;
       IAP_CMD= CMD_ERASE;
       IAP_ADDRL= addr;
       IAP_ADDRH= addr >> 8;
       IAP_TRIG= 0x5a;
       IAP_TRIG= 0xa5;
       _nop_();
       IapIdle();
}
