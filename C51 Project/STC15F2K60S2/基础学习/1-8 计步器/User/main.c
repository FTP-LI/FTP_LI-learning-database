#include <STC15F2K60S2.H>
#include "intrins.h"
#include "Delay.h"
#include "TM1637.h"

sbit BY=P0^1;
sbit KEY1=P0^0;

#define CMD_IDLE 0                  //����ģʽ
#define CMD_READ 1                  //IAP�ֽڶ�����
#define CMD_PROGRAM 2               //IAP�ֽڱ������
#define CMD_ERASE 3                 //IAP������������
#define ENABLE_IAP 0x82             //ifSYSCLK<12MHZ
#define KEY1_PRESS	1
#define KEY_NOPRESS	0
#define BY_PRESS	1
#define BY_NOPRESS	0

uint8_t D1,D2,S1,S2,S3,S4;

void IapIdle(void)//�ر�IAP

{
       IAP_CONTR= 0;                            
       IAP_CMD= 0;                              
       IAP_TRIG= 0;                             
       IAP_ADDRH= 0x80;                         
       IAP_ADDRL= 0;
}

uint8_t IapReadByte(uint16_t addr)//��ISP/IAP/EEPROM�����ȡһ���ֽ�

{
       uint8_t dat;                             
       IAP_CONTR= ENABLE_IAP;                   
       IAP_CMD= CMD_READ;                       
       IAP_TRIG= 0xa5;                          
       IAP_ADDRL= addr;                         
       IAP_ADDRH= addr >> 8;                    
       IAP_TRIG= 0x5a;                          
       _nop_();                                 
       dat= IAP_DATA;                           
       IapIdle();                               
       return dat;                              
}

void IapProgramByte(uint16_t addr, uint8_t dat)//дһ���ֽڵ�EEPROM����

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


uint8_t KEY(void)//�������
{
    if(KEY1==0)
    {
        delay_ms(15);
        if(KEY1==0)
        {
            return KEY1_PRESS;
        }
    }
    return KEY_NOPRESS;
}

uint8_t BYS(void)//�������
{
    if(BY==0)
    {
        delay_ms(20);
        if(BY==0)
        {
            return BY_PRESS;
        }
    }
    return BY_NOPRESS;
}


void main()
{
    D2 =IapReadByte(0x200);//��ROM�л�ȡ����,������λ
    S1 = D2/1000;//����ܵ�һλ
    S2 = (D2/100)%10;
    S3 = (D2/10)%10;
    S4 = D2%10;//����ܵ���λ
    TM1637_Display(S1,S2,S3,S4,0);//�ϵ���ʾ
    while(1)
    {
        if(KEY()==1)//���������º�λ
        {
            D1=0;
            IapEraseSector(0x200);
            IapProgramByte(0x200,D1);
        }
        if(BYS()==1)//�����ۼ�
        {
            D1++;
            IapEraseSector(0x200);
            IapProgramByte(0x200,D1);
        }
            D2 =IapReadByte(0x200);//��ROM�л�ȡ����,������λ
            S1 = D2/1000;//����ܵ�һλ
            S2 = (D2/100)%10;
            S3 = (D2/10)%10;
            S4 = D2%10;//����ܵ���λ
            TM1637_Display(S1,S2,S3,S4,0);//ˢ����ʾ
    }
}
