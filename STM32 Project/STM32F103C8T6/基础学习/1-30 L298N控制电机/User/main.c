#include "stm32f10x.h"                  // Device header
#include "delay.h"
#include "OLED.h"
#include "L298N.h"
#include "NRF24L01.h"
#include "sys.h"
#include "key.h"
#include "PWM.h"
#include "AD.h"
#include "MPU6050.h"
#include "Scontrol.h"

int main(void)
{   
    uint8_t m,t;
    OLED_Init();
    L298N_Init();
//    uchar A[1];
//    A[0]= 0x00;
    OLED_ShowString(1,2,"   NRF24L01");
    NRF24L01_Init();
    while(NRF24L01_Check() == 1){
        L298N_Function(0);
    }
    Send_Control_Init();
	while(1)
	{
        OLED_ShowString(1,2,"   NRF24L01");
        OLED_ShowString(2,1,"Send:");
        OLED_ShowString(3,1,"MOD:");
        OLED_ShowString(3,5,"NO");
        m = Key_GetNumB();
        while(m == 8)
        {
            Send_Control_MOD(0);
           t = Key_GetNumB();
           if(t == 11)
           {
               OLED_Clear();
               break;              
           }
        }
        while(m == 9)
        {
            OLED_ShowString(2,9,"Rocker");
            Send_Control_MOD(1);
           t = Key_GetNumB();
           if(t == 11)
           {
               OLED_Clear();
               break;
           }
        }
    }
}
//        MPU6050_GetData(&AX,&AY);
//        AX = AX/200;
//        AY = AY/200;
//        OLED_ShowString(3,1,"AX:");
//        OLED_ShowString(3,8,"AY:");
//		OLED_ShowSignedNum(3,4,AX,2);
//		OLED_ShowSignedNum(3,11,AY,2);
//        if(AX<5 && AX >-5 && AY<5 && AY > -5 )
//        {
//            SEND_BUF(E);
//            OLED_ShowHexNum(2,6,E[0],2);            
//        }
//        else if(AX < -5)
//        {
//            SEND_BUF(A);
//            OLED_ShowHexNum(2,6,A[0],2);
//        }
//        else if(AX > 5)
//        {
//            SEND_BUF(B);
//            OLED_ShowHexNum(2,6,B[0],2);
//        }
//        if(AY < -5)
//        {
//            SEND_BUF(C);
//            OLED_ShowHexNum(2,6,C[0],2);            
//        }
//        else if(AY > 5)
//        {
//            SEND_BUF(D);
//            OLED_ShowHexNum(2,6,D[0],2);
//        }//��п��Ʋ���
//	}//���Ͷ�
//}

//    while(1)
//	{
//		
//	  if( PBin(1) == 0) //�����⵽��ʼ��������
//	  {
//		NRF24L01_RxPacket(A); //�����ݷ�������A
//          OLED_ShowHexNum(2,6,A[0],2);
//	  if( PBin(1) == 0) //�����⵽��ʼ��������
//	  {
//          OLED_ShowString(3,5,"NO");
//		NRF24L01_RxPacket(A); //�����ݷ�������A
//        if(A[0] == 0x01)//ǰ��
//        {
//            L298N_Function(0);//����0Ϊ��ת
//            L298N_Speed(40);
//            A[0] = 0x00;
//          }
//          if(A[0] == 0x02)//����
//          {
//              L298N_Function(1);//����0Ϊ��ת
//              L298N_Speed(40);
//              A[0] = 0x00;
//          }
//          if(A[0] == 0x03)//��ת
//          {
//              L298N_Function(2);//����0Ϊ��ת
//              L298N_Speed(40);
//              A[0] = 0x00;
//          }
//          if(A[0] == 0x04)//��ת
//          {
//              L298N_Function(3);//����0Ϊ��ת
//              L298N_Speed(40);
//              A[0] = 0x00;    
//          }
//          if(A[0] == 0x07)//ֹͣ
//          {
//              L298N_STOP();
//              A[0] = 0x00;    
//          }
//        }          
//      }          
//	}//���Ͷ�	

//}	