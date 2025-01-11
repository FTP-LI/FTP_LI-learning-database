#include <STC15F2K60S2.H>
#include "intrins.h"
#include "Delay.h"
#include "F2481.h"

sbit BY=P1^4;
sbit KEY1=P1^5;

#define CMD_IDLE 0                  //空闲模式
#define CMD_READ 1                  //IAP字节读命令
#define CMD_PROGRAM 2               //IAP字节编程命令
#define CMD_ERASE 3                 //IAP扇区擦除命令
#define ENABLE_IAP 0x82             //ifSYSCLK<12MHZ
#define KEY1_PRESS	1
#define KEY_NOPRESS	0
#define BY_PRESS	1
#define BY_NOPRESS	0

uint8_t D1,D2,S1,S2,S3,S4;

void IapIdle(void)//关闭IAP

{
       IAP_CONTR= 0;                            
       IAP_CMD= 0;                              
       IAP_TRIG= 0;                             
       IAP_ADDRH= 0x80;                         
       IAP_ADDRL= 0;
}

uint8_t IapReadByte(uint16_t addr)//从ISP/IAP/EEPROM区域读取一个字节

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

void IapProgramByte(uint16_t addr, uint8_t dat)//写一个字节到EEPROM区域

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

void IapEraseSector(uint16_t addr)//扇区擦除

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


uint8_t KEY(void)//按键检测
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

uint8_t BYS(void)//步数检测
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
    D2 =IapReadByte(0x200);//从ROM中获取数据,步数高位
    S1 = D2/1000;//数码管第一位
    S2 = (D2/100)%10;
    S3 = (D2/10)%10;
    S4 = D2%10;//数码管第四位
    F24811_freshall(D2);//断电显示
    while(1)
    {
        F24811_freshall(D2);
        if(KEY()==1)//当按键按下后复位
        {
            D1=0;
            IapEraseSector(0x200);
            IapProgramByte(0x200,D1);
        }
        if(BYS()==1)//步数累计
        {
            D1++;
            IapEraseSector(0x200);
            IapProgramByte(0x200,D1);
        }
            D2 =IapReadByte(0x200);//从ROM中获取数据,步数高位
            F24811_freshall(D2);//刷新显示
    }
}
