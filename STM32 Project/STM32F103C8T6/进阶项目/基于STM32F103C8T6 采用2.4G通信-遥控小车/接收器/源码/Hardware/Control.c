#include "stm32f10x.h"                  // Device header
#include "Control.h"
#include <stdio.h>

void Control_Init(void)
{
    OLED_Init();
    NRF24L01_Init();
    L298N_Init();
    L298N_STOP();
}

void Control(void)
{
    uint8_t Buf[32] = {4, 20, 20, 20, 20};//����һλ���ݳ��ȣ���λģʽ��������λ��������
    while (1)
	{
		if (NRF24L01_Get_Value_Flag() == 0)
		{
			NRF24L01_GetRxBuf(Buf);
		}//���ݻ�ȡ
		OLED_ShowNum(2, 1, Buf[1], 2);
		OLED_ShowNum(2, 4, Buf[2], 2);
		OLED_ShowNum(3, 1, Buf[3], 2);
		OLED_ShowNum(3, 4, Buf[4], 2);
        if(Buf[2] >= 19 && Buf[2] <= 21 && Buf[1] >= 19 && Buf[1] <= 21 && Buf[3] >= 19 && Buf[3] <= 21)
        {
            L298N_STOP();
        }
        else if(Buf[2] > 21)//ǰ��
        {
            L298N_Function(0);
            L298N_Speed((Buf[2]-21)*3.8+30);
        }
        else if(Buf[2] < 19)//����
        {
            L298N_Function(1);
            L298N_Speed((19-Buf[2])*3.8+30);
        }
        if(Buf[1] > 21)//��ƽ��
        {
            L298N_Function(2);
            L298N_Speed((Buf[1]-21)*3.8+30);
        } 
        else if(Buf[1] < 19)//��ƽ��
        {
            L298N_Function(3);
            L298N_Speed((19-Buf[1])*3.8+30);
        }
        if(Buf[3] < 19)//����ת
        {
            L298N_Function(4);
            L298N_Speed((19-Buf[3])*3.8+30);
        }
        else if(Buf[3] > 21)//����ת
        {
            L298N_Function(5);
            L298N_Speed((Buf[3]-21)*3.8+30);
        }        
	}
}
