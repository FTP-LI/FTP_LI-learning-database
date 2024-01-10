#ifndef _DS1302_H_
#define _DS1302_H_

//---包含头文件---//
#include <STC15F2K60S2.H>
#include <intrins.h>
#include "Sys.h"

#define DS1302_write		0x00				//写
#define DS1302_read			0x01				//读

#define DS1302_second			0X80		    //秒寄存器
#define DS1302_minute			0X82		    //分寄存器
#define DS1302_hour  			0X84		    //小时寄存器
#define DS1302_day   			0X86		    //日寄存器
#define DS1302_mouth			0X88		    //月寄存器
#define DS1302_week				0X8A		    //星期寄存器
#define DS1302_year				0X8C		    //年寄存器

#define DS1302_write_protect	0X8E		    //写保护寄存器
#define DS1302_Man				0X90		    //慢充电寄存器

void Ds1302_Write(uint8_t addr, uint8_t dat);   //向DS1302命令（地址+数据）
uint8_t Ds1302_Read(uint8_t addr);              //读取一个地址的数据
void Ds1302_Init(void);
void DS1302_SetTime(void);
void DS1302_ReadTime(void);

///************************ 修改时间函数 ************************************/
//// mode		0	1	2	3	4	5	6
//// 模式		秒	分	时	日	月	年	周
///*************************************************************************/
//// sign		1	0
//// 正负		+	-
///*************************************************************************/
//void Change_time(uint8_t mode, bit sign);

#endif