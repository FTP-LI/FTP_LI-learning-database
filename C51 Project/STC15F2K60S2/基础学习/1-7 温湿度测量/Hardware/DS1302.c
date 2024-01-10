#include "ds1302.h"

sbit SCLK=P0^5;
sbit DAT=P0^6;
sbit RST=P0^7;
//�ӿڶ���

uint8_t code RTC_ADDR[7] = {DS1302_year, DS1302_mouth, DS1302_week, DS1302_day, DS1302_hour, DS1302_minute, DS1302_second};

uint8_t TIME_FIR[7] = {24, 1, 1, 1, 0, 0, 0};           //��ʼʱ���飬��ʼ��ʱ��2024-1-1����һ0��0��0��
uint8_t DS1302_Time[7];                                 //��ʾʱ���飬�û�����ʹ��extern����
//ʱ���ʼֵ���������� �� �� �� �� ʱ �� ��

void Ds1302_Write(uint8_t addr, uint8_t dat) //��DS1302�����ַ+���ݣ�
{
	uint8_t n;
	RST = 0;
	_nop_();
	SCLK = 0;                               //�Ƚ�SCLK�õ͵�ƽ��
	_nop_();
	RST = 1;                                //Ȼ��RST(CE)�øߵ�ƽ��
	_nop_();
	for (n=0; n<8; n++)                     //��ʼ���Ͱ�λ��ַ����
	{
		DAT = addr & 0x01;                  //���ݴӵ�λ��ʼ����
		addr >>= 1;
		SCLK = 1;                           //������������ʱ��DS1302��ȡ����
		_nop_();
		SCLK = 0;
		_nop_();
	}
	for (n=0; n<8; n++)                     //д��8λ����
	{
		DAT = dat & 0x01;
		dat >>= 1;
		SCLK = 1;                           //������������ʱ��DS1302��ȡ����
		_nop_();
		SCLK = 0;
		_nop_();	
	}	
	RST = 0;                                //�������ݽ���
	_nop_();
}

uint8_t Ds1302_Read(uint8_t addr)                //��ȡһ����ַ������
{
	uint8_t n,dat,dat1;
    addr|=0x01;                             //��д��ַת��Ϊ����ַ
	RST = 0;
	_nop_();
	SCLK = 0;                               //�Ƚ�SCLK�õ͵�ƽ��
	_nop_();
	RST = 1;                                //Ȼ��RST(CE)�øߵ�ƽ��
	_nop_();
	for(n=0; n<8; n++)                      //��ʼ���Ͱ�λ��ַ����
	{
		DAT = addr & 0x01;                  //���ݴӵ�λ��ʼ����
		addr >>= 1;
		SCLK = 1;                           //������������ʱ��DS1302��ȡ����
		_nop_();
		SCLK = 0;                           //DS1302�½���ʱ����������
		_nop_();
	}
	_nop_();
	for(n=0; n<8; n++)                      //��ȡ8λ����
	{
		dat1 = DAT;                         //�����λ��ʼ����
		dat = (dat>>1) | (dat1<<7);
		SCLK = 1;
		_nop_();
		SCLK = 0;                           //DS1302�½���ʱ����������
		_nop_();
	}
	RST = 0;
	_nop_();	                            //����ΪDS1302��λ���ȶ�ʱ��,����ġ�
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

void DS1302_SetTime(void)//����ʱ��,ʮ����תBCD���д��
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
}//�ӡ�0~6���ֱ����ꡢ�¡��ܡ��ա�ʱ���֡���

void DS1302_ReadTime(void)//��ȡʱ��,BCD��תʮ���ƺ��ȡ
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



///************************ �޸�ʱ�亯�� ************************************/
//// mode		0	1	2	3	4	5	6
//// ģʽ		��	��	ʱ	��	��	��	��
///*************************************************************************/
//// sign		1	0
//// ����		+	-
///*************************************************************************/
//void Change_time(uchar mode, bit sign)
//{	
//	switch(mode)
//	{
//		// �޸� ��
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
//		// �޸� ����
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
//		// �޸� Сʱ
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
//		// �޸� ��
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
//		// �޸� ��
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
//		// �޸� ��
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
//		// �޸� ��
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
