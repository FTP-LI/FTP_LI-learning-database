#include <STC15F2K60S2.H>
#include "intrins.h"
#include "E2ROM.h"

#define CMD_IDLE 0                  //空闲模式
#define CMD_READ 1                  //IAP字节读命令
#define CMD_PROGRAM 2               //IAP字节编程命令
#define CMD_ERASE 3                 //IAP扇区擦除命令
#define ENABLE_IAP 0x82             //ifSYSCLK<12MHZ
#define IAP_ADDRESS 0x0000
#define Part_1 0x0000
#define Part_2 0x0200
#define Part_3 0x600

//可用扇区地址 0x0000 0x0200 0x600 这里列举三个



void IapIdle(void)//关闭IAP

{
       IAP_CONTR= 0;                            //关闭IAP功能
       IAP_CMD= 0;                              //清除指令待机
       IAP_TRIG= 0;                             //清空触发器寄存器
       IAP_ADDRH= 0x80;                         //将地址设置到非IAP区域
       IAP_ADDRL= 0;
}

uint8_t IapReadByte(uint16_t addr)//从ISP/IAP/EEPROM区域读取一个字节

{
       uint8_t dat;                             //数据缓冲区
       IAP_CONTR= ENABLE_IAP;                   //使能IAP
       IAP_CMD= CMD_READ;                       //写触发命令（0x5a）
       IAP_TRIG= 0xa5;                          //设置读取命令
       IAP_ADDRL= addr;                         //设置IAP低八位地址
       IAP_ADDRH= addr >> 8;                    //设置IAP高八位地址  
       IAP_TRIG= 0x5a;                          //写触发命令（0xa5）
       _nop_();                                 //等待ISP/IAP/EEPROM操作完成   
       dat= IAP_DATA;                           //读ISP/IAP/EEPROM数据
       IapIdle();                               //关闭IAP功能
       return dat;                              //返回

}

void IapProgramByte(uint16_t addr, uint8_t dat)//写一个字节到ISP/IAP/EEPROM区域

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
