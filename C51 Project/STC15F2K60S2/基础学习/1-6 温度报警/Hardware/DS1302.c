#include "ds1302.h"

sbit SCLK=P0^5;
sbit DAT=P0^6;
sbit RST=P0^7;
//接口定义

uint8_t code RTC_ADDR[7] = {DS1302_year, DS1302_mouth, DS1302_week, DS1302_day, DS1302_hour, DS1302_minute, DS1302_second};

uint8_t TIME_FIR[7] = {24, 1, 1, 1, 0, 0, 0};           //初始时间组，初始化时间2024-1-1星期一0点0分0秒
uint8_t DS1302_Time[7];                                 //显示时间组，用户函数使用extern调用
//时间初始值，从左往右 年 月 周 日 时 分 秒

void Ds1302_Write(uint8_t addr, uint8_t dat) //向DS1302命令（地址+数据）
{
	uint8_t n;
	RST = 0;
	_nop_();
	SCLK = 0;                               //先将SCLK置低电平。
	_nop_();
	RST = 1;                                //然后将RST(CE)置高电平。
	_nop_();
	for (n=0; n<8; n++)                     //开始传送八位地址命令
	{
		DAT = addr & 0x01;                  //数据从低位开始传送
		addr >>= 1;
		SCLK = 1;                           //数据在上升沿时，DS1302读取数据
		_nop_();
		SCLK = 0;
		_nop_();
	}
	for (n=0; n<8; n++)                     //写入8位数据
	{
		DAT = dat & 0x01;
		dat >>= 1;
		SCLK = 1;                           //数据在上升沿时，DS1302读取数据
		_nop_();
		SCLK = 0;
		_nop_();	
	}	
	RST = 0;                                //传送数据结束
	_nop_();
}

uint8_t Ds1302_Read(uint8_t addr)                //读取一个地址的数据
{
	uint8_t n,dat,dat1;
    addr|=0x01;                             //将写地址转换为读地址
	RST = 0;
	_nop_();
	SCLK = 0;                               //先将SCLK置低电平。
	_nop_();
	RST = 1;                                //然后将RST(CE)置高电平。
	_nop_();
	for(n=0; n<8; n++)                      //开始传送八位地址命令
	{
		DAT = addr & 0x01;                  //数据从低位开始传送
		addr >>= 1;
		SCLK = 1;                           //数据在上升沿时，DS1302读取数据
		_nop_();
		SCLK = 0;                           //DS1302下降沿时，放置数据
		_nop_();
	}
	_nop_();
	for(n=0; n<8; n++)                      //读取8位数据
	{
		dat1 = DAT;                         //从最低位开始接收
		dat = (dat>>1) | (dat1<<7);
		SCLK = 1;
		_nop_();
		SCLK = 0;                           //DS1302下降沿时，放置数据
		_nop_();
	}
	RST = 0;
	_nop_();	                            //以下为DS1302复位的稳定时间,必须的。
	SCLK = 1;
	_nop_();
	DAT = 0;
	_nop_();
	DAT = 1;
	_nop_();
	return dat;	
}

void Ds1302_Init(void)
{
	Ds1302_Write(DS1302_write_protect,DS1302_write);
	Ds1302_Write(DS1302_year,TIME_FIR[0]/10*16+TIME_FIR[0]%10);               
	Ds1302_Write(DS1302_mouth,TIME_FIR[1]/10*16+TIME_FIR[1]%10);
	Ds1302_Write(DS1302_week,TIME_FIR[2]/10*16+TIME_FIR[2]%10);
	Ds1302_Write(DS1302_day,TIME_FIR[3]/10*16+TIME_FIR[3]%10);
	Ds1302_Write(DS1302_hour,TIME_FIR[4]/10*16+TIME_FIR[4]%10);
	Ds1302_Write(DS1302_minute,TIME_FIR[5]/10*16+TIME_FIR[5]%10);
	Ds1302_Write(DS1302_second,TIME_FIR[6]/10*16+TIME_FIR[6]%10);
	Ds1302_Write(DS1302_write_protect,DS1302_read);
}

void DS1302_SetTime(void)//设置时间,十进制转BCD码后写入
{
	Ds1302_Write(DS1302_write_protect,DS1302_write);
	Ds1302_Write(DS1302_year,DS1302_Time[0]/10*16+DS1302_Time[0]%10);               
	Ds1302_Write(DS1302_mouth,DS1302_Time[1]/10*16+DS1302_Time[1]%10);
	Ds1302_Write(DS1302_week,DS1302_Time[2]/10*16+DS1302_Time[2]%10);
	Ds1302_Write(DS1302_day,DS1302_Time[3]/10*16+DS1302_Time[3]%10);
	Ds1302_Write(DS1302_hour,DS1302_Time[4]/10*16+DS1302_Time[4]%10);
	Ds1302_Write(DS1302_minute,DS1302_Time[5]/10*16+DS1302_Time[5]%10);
	Ds1302_Write(DS1302_second,DS1302_Time[6]/10*16+DS1302_Time[6]%10);
	Ds1302_Write(DS1302_write_protect,DS1302_read);
}//从【0~6】分别是年、月、周、日、时、分、秒

void DS1302_ReadTime(void)//读取时间,BCD码转十进制后读取
{
	unsigned char Temp;
	Temp=Ds1302_Read(DS1302_year);
	DS1302_Time[0]=Temp/16*10+Temp%16;
	Temp=Ds1302_Read(DS1302_mouth);
	DS1302_Time[1]=Temp/16*10+Temp%16;
	Temp=Ds1302_Read(DS1302_week);
	DS1302_Time[2]=Temp/16*10+Temp%16;
	Temp=Ds1302_Read(DS1302_day);
	DS1302_Time[3]=Temp/16*10+Temp%16;
	Temp=Ds1302_Read(DS1302_hour);
	DS1302_Time[4]=Temp/16*10+Temp%16;
	Temp=Ds1302_Read(DS1302_minute);
	DS1302_Time[5]=Temp/16*10+Temp%16;
	Temp=Ds1302_Read(DS1302_second);
	DS1302_Time[6]=Temp/16*10+Temp%16;
}



///************************ 修改时间函数 ************************************/
//// mode		0	1	2	3	4	5	6
//// 模式		秒	分	时	日	月	周	年
///*************************************************************************/
//// sign		1	0
//// 正负		+	-
///*************************************************************************/
//void Change_time(uchar mode, bit sign)
//{	
//	switch(mode)
//	{
//		// 修改 秒
//		case 0:
//			if(sign)
//			{				
//				TIME[0]++;
//				if( (TIME[0] % 16) > 9)
//					TIME[0] += 6;	
//				if(TIME[0] > 0x59)
//					TIME[0] = 0x00;									
//			}
//			else
//			{				
//				TIME[0]--;
//				if( (TIME[0] % 16) > 9)
//					TIME[0] -= 6;
//				if(TIME[0] == 0x00)
//					TIME[0] = 0x59;
//				
//			}			
//			Ds1302Init();
//			break;
//			
//		// 修改 分钟
//		case 1:
//			if(sign)
//			{				
//				TIME[1]++;
//				if( (TIME[1] % 16) > 9)
//					TIME[1] += 6;	
//				if(TIME[1] > 0x59)
//					TIME[1] = 0x00;									
//			}
//			else
//			{				
//				TIME[1]--;
//				if( (TIME[1] % 16) > 9)
//					TIME[1] -= 6;
//				if(TIME[1] == 0x00)
//					TIME[1] = 0x59;
//				
//			}			
//			Ds1302Init();
//			break;
//			
//		// 修改 小时
//		case 2:
//			if(sign)
//			{				
//				TIME[2]++;
//				if( (TIME[2] % 16) > 9)
//					TIME[2] += 6;	
//				if(TIME[2] > 0x23)
//					TIME[2] = 0x00;									
//			}
//			else
//			{				
//				TIME[2]--;
//				if( (TIME[2] % 16) > 9)
//					TIME[2] -= 6;
//				if(TIME[2] == 0x00)
//					TIME[2] = 0x23;
//				
//			}			
//			Ds1302Init();
//			break;
//			
//		// 修改 日
//		case 3:
//			if(sign)
//			{				
//				TIME[3]++;
//				if( (TIME[3] % 16) > 9)
//					TIME[3] += 6;	
//				if(TIME[3] > 0x31)
//					TIME[3] = 0x00;									
//			}
//			else
//			{				
//				TIME[3]--;
//				if( (TIME[3] % 16) > 9)
//					TIME[3] -= 6;
//				if(TIME[3] == 0x00)
//					TIME[3] = 0x31;
//				
//			}			
//			Ds1302Init();
//			break;
//			
//		// 修改 月
//		case 4:
//			if(sign)
//			{				
//				TIME[4]++;
//				if( (TIME[4] % 16) > 9)
//					TIME[4] += 6;	
//				if(TIME[4] > 0x12)
//					TIME[4] = 0x00;									
//			}
//			else
//			{				
//				TIME[4]--;
//				if( (TIME[4] % 16) > 9)
//					TIME[4] -= 6;
//				if(TIME[4] == 0x00)
//					TIME[4] = 0x12;
//				
//			}			
//			Ds1302Init();
//			break;
//			
//		// 修改 周
//		case 5:
//			if(sign)
//			{				
//				TIME[5]++;
//				if( (TIME[5] % 16) > 9)
//					TIME[5] += 6;	
//				if(TIME[5] > 0x07)
//					TIME[5] = 0x00;									
//			}
//			else
//			{				
//				TIME[5]--;
//				if( (TIME[5] % 16) > 9)
//					TIME[5] -= 6;
//				if(TIME[5] == 0x00)
//					TIME[5] = 0x07;
//				
//			}			
//			Ds1302Init();
//			break;
//			
//		// 修改 年
//		case 6:
//			if(sign)
//			{				
//				TIME[6]++;
//				if( (TIME[6] % 16) > 9)
//					TIME[6] += 6;	
//				if(TIME[6] > 0x99)
//					TIME[6] = 0x00;									
//			}
//			else
//			{				
//				TIME[6]--;
//				if( (TIME[6] % 16) > 9)
//					TIME[6] -= 6;
//				if(TIME[6] == 0x00)
//					TIME[6] = 0x99;
//				
//			}			
//			Ds1302Init();
//			break;
//		default:
//			break;
//	}
//}
