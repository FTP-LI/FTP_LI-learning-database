#include "stm32f10x.h"                  // Device header
#include "NRF24L01.h"
#include "Key.h"
#include "sys.h"
#include "AD.h"
#include "MPU6050.h"
#include "OLED.h"
#include "Scontrol.h"
#include "Delay.h"
//�ú���Ϊ���Ͷ˿��ƺ���
uint8_t i,j;
uchar A[1],B[1],C[1],D[1],E[1],F[1],G[1];//ָ�
int16_t AX,AY;//��п���
uint8_t X1,Y1,X2,Y2;//ң�ؿ���

void Send_Control_Init(void)//����ģ���ʼ��
{
    NRF24L01_Init();
    Key_Init();
    AD_Init();
    MPU6050_Init();
    A[0]=0x01,B[0]=0x02,C[0]=0x03,D[0]=0x04,E[0]=0x05,F[0]=0x06,G[0]=0x07;//�ֱ��Ӧǰ��A������B����תC����תD����ƽ��E����ƽ��F��ֹͣG
    while(NRF24L01_Check() == 1);//���NRF24L01�Ƿ����
}

void Send_Control_MOD(uint8_t MOD)
{
    if(MOD == 0)//����ģʽ
    {
        i = Key_GetNumA();
        OLED_ShowString(3,5,"Key-Press");
        if(i == 1)
        {
            SEND_BUF(A);
            OLED_ShowHexNum(2,6,A[0],2);
        }//ǰ��
        if(i == 2)
        {
            SEND_BUF(B);
            OLED_ShowHexNum(2,6,B[0],2);
        }//����
        if(i == 3)
        {
            SEND_BUF(C);
            OLED_ShowHexNum(2,6,C[0],2);
        }//��ת
        if(i == 4)
        {
            SEND_BUF(D);
            OLED_ShowHexNum(2,6,D[0],2);
        }//��ת
        if(i == 5)
        {
            SEND_BUF(E);
            OLED_ShowHexNum(2,6,E[0],2);
        }//��ƽ��
        if(i == 6)
        {
            SEND_BUF(F);
            OLED_ShowHexNum(2,6,F[0],2);
        }//��ƽ��
        if(i == 7)
        {
            SEND_BUF(G);
            OLED_ShowHexNum(2,6,G[0],2);
        }//ֹͣ        
    }
    else if(MOD == 1)//ҡ��ģʽ
    {
        X1 = AD_Value[0]/400;
        Y1 = AD_Value[1]/400;
        X2 = AD_Value[2]/400;
        Y2 = AD_Value[3]/400;
        OLED_ShowString(3,1,"X1:");
        OLED_ShowString(3,8,"Y1:");
        OLED_ShowString(4,1,"X2:");
        OLED_ShowString(4,8,"Y2:");
        OLED_ShowString(3,6," ");
        OLED_ShowNum(3,4,X1,2);
        OLED_ShowNum(3,11,Y1,2);
        OLED_ShowNum(4,4,X2,2);
        OLED_ShowNum(4,11,Y2,2);
        
        if(X1 >4 && X1 < 6 && Y1 < 6 && Y1 > 4 &&Y2 < 6 && Y2 > 4)
        {
            SEND_BUF(G);
            OLED_ShowHexNum(2,6,G[0],2);            
        }//ֹͣ
        if(X1 > 6)
        {
            SEND_BUF(A);
            OLED_ShowHexNum(2,6,A[0],2);
        }//ǰ��
        if(X1 <4)
        {
            SEND_BUF(B);
            OLED_ShowHexNum(2,6,B[0],2);
        }//����
        if(Y1 < 4)
        {
            SEND_BUF(C);
            OLED_ShowHexNum(2,6,C[0],2);            
        }//��ת
        if(Y1 > 6)
        {
            SEND_BUF(D);
            OLED_ShowHexNum(2,6,D[0],2);
        }//��ת
        if(Y2 > 6)
        {
            SEND_BUF(D);
            OLED_ShowHexNum(2,6,E[0],2);
        }//��ƽ��
        if(Y2 > 6)
        {
            SEND_BUF(D);
            OLED_ShowHexNum(2,6,F[0],2);
        }//��ƽ��
    }
    else if(MOD == 2)//��п���ģʽ
    {
        
    }
}
