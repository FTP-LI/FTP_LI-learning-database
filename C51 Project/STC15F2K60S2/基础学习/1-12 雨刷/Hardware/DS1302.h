#ifndef _DS1302_H_
#define _DS1302_H_

//---����ͷ�ļ�---//
#include <STC15F2K60S2.H>
#include <intrins.h>
#include "Sys.h"

#define DS1302_write		0x00				//д
#define DS1302_read			0x01				//��

#define DS1302_second			0X80		    //��Ĵ���
#define DS1302_minute			0X82		    //�ּĴ���
#define DS1302_hour  			0X84		    //Сʱ�Ĵ���
#define DS1302_day   			0X86		    //�ռĴ���
#define DS1302_mouth			0X88		    //�¼Ĵ���
#define DS1302_week				0X8A		    //���ڼĴ���
#define DS1302_year				0X8C		    //��Ĵ���

#define DS1302_write_protect	0X8E		    //д�����Ĵ���
#define DS1302_Man				0X90		    //�����Ĵ���

void Ds1302_Write(uint8_t addr, uint8_t dat);   //��DS1302�����ַ+���ݣ�
uint8_t Ds1302_Read(uint8_t addr);              //��ȡһ����ַ������
void Ds1302_Init(void);
void DS1302_SetTime(void);
void DS1302_ReadTime(void);

///************************ �޸�ʱ�亯�� ************************************/
//// mode		0	1	2	3	4	5	6
//// ģʽ		��	��	ʱ	��	��	��	��
///*************************************************************************/
//// sign		1	0
//// ����		+	-
///*************************************************************************/
//void Change_time(uint8_t mode, bit sign);

#endif