#include "stm32f10x.h"                  // Device header
#include "delay.h"
#include "OLED.h"
#include "NRF24L01.h"
#include "sys.h"

int main(void)
{
	uchar A[32],C[32] ;
    A[0] = 0x55;
//    C[0] = 0x01;

	OLED_Init();
	NRF24L01_Init();
    OLED_ShowString(1,4," NRF24L01 ");
	OLED_ShowString(2,1,"Send:");
    OLED_ShowHexNum(2,6,A[0],2);
    
	while(1)
	{
//        SEND_BUF(A);
//        SEND_BUF(C);
//	}//���Ͷ�
    
//    while(1)
//	{
//		
	  if( PBin(1) == 0) //�����⵽��ʼ��������
	  {
		NRF24L01_RxPacket(C); //�����ݷ�������C
	  }
	  
	    OLED_ShowHexNum(2,6,C[0],2);
	}//���Ͷ�		
}	
