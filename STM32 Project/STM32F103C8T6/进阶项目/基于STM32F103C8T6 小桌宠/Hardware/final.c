#include "stm32f10x.h"                  // Device header
#include "Servo.h"
#include "NRF24L01.h"
#include "OLED.h"
#include "key.h"
#include "final.h"
#include "Delay.h"
#include "LED.h"

void Final_Init(void)
{
    OLED_Init();//OLED
    Servo_Init();//���
    LED_Init();//LED
    NRF24L01_Init();//2.4G����ͨ��ģ��
    move_chek();//�˶����
    LED_ALL_W(0);//��ȫ��
}

void crol(void)
{
    uint8_t Buf[32] = {4, 20, 20, 20, 20};//����һλ���ݳ��ȣ���λģʽ��������λ��������
    /*
    ���Ա�� 
    Buf[1]����ҡ����40��0
    Buf[2]����ҡ����40��0
    Buf[3]����ҡ����0��40
    Buf[4]����ҡ����40��0
    */
    while(1)//����������
    {
        OLED_ShowMyPicture(3,32,32,48,0);
        OLED_ShowMyPicture(3,64,32,48,0);
//        OLED_Show_eyes_round();
        uint8_t flag=1;
        if (NRF24L01_Get_Value_Flag() == 0)
		{
			NRF24L01_GetRxBuf(Buf);
		}//���ݻ�ȡ
        if(Buf[1]>10&&Buf[2]>25&&Buf[4]>25)//�ڶ�
        {
            move_shakebody();
            Buf[2] = 20;
            Buf[4] = 20;
        }
        if(Buf[1]>10&&Buf[2]<15&&Buf[4]<15)//����
        {
            move_shakehand();
            Buf[2] = 20;
            Buf[4] = 20;
        }
        if(Buf[1]<25)//����LED����
        {
            flag=flag%2;
            LED_ALL_W(flag);
            flag++;
        }
        if(Buf[2]>25)//ǰ��
        {
            move_headpack(30);
        }
        if(Buf[2]<15)//����
        {
            move_backpack(30);
        }
        if(Buf[3]<15)//��ת
        {
            move_leftpack(30);
        }
        if(Buf[3]>25)//��ת
        {
            move_rightpack(30);
        }
    }
}

void crol_L_H(float Angle)//��ǰ
{
    Servo1_SetAngle(Angle);
}

void crol_L_L(float Angle)//���
{
    Servo2_SetAngle(15+Angle);
}
void crol_R_H(float Angle)//��ǰ
{
    Servo4_SetAngle(175-Angle);//���ֵ����
}
void crol_R_L(float Angle)//�Һ�
{
    Servo3_SetAngle(175-Angle);
}

void move_chek(void)
{
    move_rest();//׼����̬
    move_restop();//��׼���ظ�ֹͣ
}

void move_headpack(uint8_t Angle)//ǰ����̬����
{
    OLED_Show_eyes_crash();
    crol_L_H(90);
    crol_R_H(90-Angle);
    crol_L_L(90-Angle);
    crol_R_L(90);
        Delay_ms(Angle*3);
    crol_L_H(90+Angle);
    crol_R_H(90-Angle);
    crol_L_L(90-Angle);
    crol_R_L(90+Angle);
        Delay_ms(Angle*3);
    crol_L_H(90);
    crol_R_H(90);
    crol_L_L(90);
    crol_R_L(90);
        Delay_ms(Angle*3);
    crol_L_H(90-Angle);
    crol_R_H(90);
    crol_L_L(90);
    crol_R_L(90-Angle);
        Delay_ms(Angle*3);
    crol_L_H(90-Angle);
    crol_R_H(90+Angle);
    crol_L_L(90+Angle);
    crol_R_L(90-Angle);
        Delay_ms(Angle*3);
    crol_L_H(90);
    crol_R_H(90);
    crol_L_L(90);
    crol_R_L(90);
        Delay_ms(Angle*3);
    OLED_Clear();
}

void move_backpack(uint8_t Angle)//���˲�̬����
{
    OLED_Show_eyes_crash();
    crol_L_H(90);
    crol_R_H(90+Angle);
    crol_L_L(90+Angle);
    crol_R_L(90);
        Delay_ms(Angle*3);
    crol_L_H(90-Angle);
    crol_R_H(90+Angle);
    crol_L_L(90+Angle);
    crol_R_L(90-Angle);
        Delay_ms(Angle*3);
    crol_L_H(90);
    crol_R_H(90);
    crol_L_L(90);
    crol_R_L(90);
        Delay_ms(Angle*3);
    crol_L_H(90+Angle);
    crol_R_H(90);
    crol_L_L(90);
    crol_R_L(90+Angle);
        Delay_ms(Angle*3);
    crol_L_H(90+Angle);
    crol_R_H(90-Angle);
    crol_L_L(90-Angle);
    crol_R_L(90+Angle);
        Delay_ms(Angle*3);
    crol_L_H(90);
    crol_R_H(90);
    crol_L_L(90);
    crol_R_L(90);
        Delay_ms(Angle*3);
    OLED_Clear();
}

void move_rightpack(uint8_t Angle)//��ת��̬����
{
    OLED_Show_eyes_crash();
    crol_L_H(90);
    crol_R_H(90-Angle);
    crol_L_L(90+Angle);
    crol_R_L(90);
        Delay_ms(Angle*5);
    crol_L_H(90+Angle);
    crol_R_H(90-Angle);
    crol_L_L(90+Angle);
    crol_R_L(90-Angle);
        Delay_ms(Angle*5);
    crol_L_H(90+Angle);
    crol_R_H(90);
    crol_L_L(90);
    crol_R_L(90-Angle);
        Delay_ms(Angle*5);
    crol_L_H(90);
    crol_R_H(90);
    crol_L_L(90);
    crol_R_L(90);
        Delay_ms(Angle*5);
    OLED_Clear();
}

void move_leftpack(uint8_t Angle)//��ת��̬����
{
    OLED_Show_eyes_crash();
    crol_L_H(90);
    crol_R_H(90+Angle);
    crol_L_L(90-Angle);
    crol_R_L(90);
        Delay_ms(Angle*5);
    crol_L_H(90-Angle);
    crol_R_H(90+Angle);
    crol_L_L(90-Angle);
    crol_R_L(90+Angle);
        Delay_ms(Angle*5);
    crol_L_H(90-Angle);
    crol_R_H(90);
    crol_L_L(90);
    crol_R_L(90+Angle);
        Delay_ms(Angle*5);
    crol_L_H(90);
    crol_R_H(90);
    crol_L_L(90);
    crol_R_L(90);
        Delay_ms(Angle*5);
    OLED_Clear();
}

void move_shakehand()//���ֺ���
{
    uint8_t i;
    OLED_Show_eyes_confot();
    crol_L_H(90);  
    crol_R_H(90);
    crol_L_L(90);    
    crol_R_L(90);
    Delay_ms(540);
    crol_L_H(45);  
    crol_R_H(45);
    crol_L_L(45);    
    crol_R_L(45);
    Delay_ms(135);
    crol_L_H(100);  
    crol_R_H(100);
    crol_L_L(45);    
    crol_R_L(45);
    Delay_ms(135);
    for(i=0;i<4;i++)
    {
        crol_R_H(0);
        Delay_ms(400);
        crol_R_H(60);
        Delay_ms(400);
    }
    move_restop();
    OLED_Clear();
}

void move_shakebody(void)//ҡ�ں���
{
    OLED_Show_eyes_confot();
    crol_L_H(120);
    crol_R_H(120);
    crol_L_L(120);
    crol_R_L(120);
        Delay_ms(150);
    crol_L_H(90);
    crol_R_H(90);
    crol_L_L(90);
    crol_R_L(90);
        Delay_ms(150);
    crol_L_H(60);
    crol_R_H(60);
    crol_L_L(60);
    crol_R_L(60);
        Delay_ms(150);
    crol_L_H(90);
    crol_R_H(90);
    crol_L_L(90);
    crol_R_L(90);
        Delay_ms(150);
    OLED_Clear();
}
void move_restop(void)
{
    Delay_ms(540);
    crol_L_H(45);  
    crol_R_H(45);
    crol_L_L(35);    
    crol_R_L(35);
    Delay_ms(540);
    crol_L_H(25);  
    crol_R_H(25);
    Delay_ms(60);
    crol_L_H(90);
    crol_L_L(90);
    crol_R_H(90);
    crol_R_L(90);
    Delay_ms(195);
}//���ʵ�鷢��ÿ�ƶ�1���ԼҪ3ms

void move_stop(void)
{
    crol_L_H(90);
    crol_L_L(90);
    crol_R_H(90);
    crol_R_L(90);
    Delay_ms(195);
}

void move_rest(void)
{
    crol_L_H(90);  
    crol_R_H(90);
    crol_L_L(90);    
    crol_R_L(90);
    Delay_ms(540);
    crol_L_H(45);  
    crol_R_H(45);
    crol_L_L(45);    
    crol_R_L(45);
    Delay_ms(135);
    crol_L_H(100);  
    crol_R_H(100);
    crol_L_L(45);    
    crol_R_L(45);
    Delay_ms(135);    
}

void move_gohead(void)
{
    crol_L_H(90);
    crol_R_H(50);
    crol_L_L(50);
    crol_R_L(90);
        Delay_ms(120);
    crol_L_H(130);
    crol_R_H(50);
    crol_L_L(50);
    crol_R_L(130);
        Delay_ms(120);
    crol_L_H(90);
    crol_R_H(90);
    crol_L_L(90);
    crol_R_L(90);
        Delay_ms(120);
    crol_L_H(50);
    crol_R_H(90);
    crol_L_L(90);
    crol_R_L(50);
        Delay_ms(120);
    crol_L_H(50);
    crol_R_H(130);
    crol_L_L(130);
    crol_R_L(50);
        Delay_ms(120);
    crol_L_H(90);
    crol_R_H(90);
    crol_L_L(90);
    crol_R_L(90);
        Delay_ms(120);    
}//���Ժ������ɸú�����д��move_headpack����

void move_goback(void)
{
    crol_L_H(90);
    crol_R_H(130);
    crol_L_L(130);
    crol_R_L(90);
        Delay_ms(120);
    crol_L_H(50);
    crol_R_H(130);
    crol_L_L(130);
    crol_R_L(50);
        Delay_ms(120);
    crol_L_H(90);
    crol_R_H(90);
    crol_L_L(90);
    crol_R_L(90);
        Delay_ms(120);
    crol_L_H(90);
    crol_R_H(50);
    crol_L_L(50);
    crol_R_L(90);
        Delay_ms(120);
    crol_L_H(130);
    crol_R_H(50);
    crol_L_L(50);
    crol_R_L(130);
        Delay_ms(120);
    crol_L_H(90);
    crol_R_H(90);
    crol_L_L(90);
    crol_R_L(90);
        Delay_ms(120); 
}//���Ժ������ɸú�����д��move_backpack����

void move_right(void)
{
    crol_L_H(80);
    crol_R_H(50);
    crol_L_L(130);
    crol_R_L(80);
        Delay_ms(120);
    crol_L_H(130);
    crol_R_H(50);
    crol_L_L(130);
    crol_R_L(50);
        Delay_ms(120);
    crol_L_H(130);
    crol_R_H(80);
    crol_L_L(80);
    crol_R_L(50);
        Delay_ms(120);
    crol_L_H(80);
    crol_R_H(80);
    crol_L_L(80);
    crol_R_L(80);
        Delay_ms(120);
}

void OLED_Show_eyes_round(void)
{
    uint8_t i,j=20;
    OLED_Clear();
    for(i=1;i<64;i++)
    {   
    OLED_ShowMyPicture(3,i,32,48,0);
    OLED_ShowMyPicture(3,i+31,32,48,0);
    Delay_ms(j);
    }
    OLED_Clear();
    for(i=1;i<64;i++)
    {   
    OLED_ShowMyPicture(3,96-i,32,48,0);
    OLED_ShowMyPicture(3,65-i,32,48,0);
    Delay_ms(j);
    }
    OLED_Clear();      
}//������ֻ�������

void OLED_Show_eyes_crash(void)
{
    OLED_Clear();
    OLED_ShowPicture(1,1,128,64,0);
}//������� "> <"

void OLED_Show_eyes_confot(void)
{
    OLED_Clear();
    OLED_ShowPicture(1,1,128,64,1);
}//������ʾ ��-��- ��





