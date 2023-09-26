#include "stm32f10x.h"                  // Device header
#include "delay.h"
#include "OLED.h"
#include "Servo.h"
#include "key.h"

int8_t KeyNum;//8λȫ�ֱ���
	
int main(void)
{
    float Angle1,Angle2;
    int i,j;
	OLED_Init();
	Servo_Init();
	Key_Init();
	
	Servo_SetAngle1(90);//�ϲ�����ֵ��С���
    Servo_SetAngle2(90);//�²�����ֵ��С����
	//�����ʼ״̬
	OLED_ShowString( 1 , 1 , "Angle1:" );
    OLED_ShowString( 2 , 1 , "Angle2:" );
	
	
	while(1)
	{
        if(Key_GetNum()==1)//Ԥ�����һ
        {
            Angle1=20;
            Angle2=0;
            Servo_SetAngle1(Angle1);
            Servo_SetAngle2(Angle2);
            Delay_ms(700);
            Angle1=90;
            Angle2=90;
            Servo_SetAngle1(Angle1);
            Servo_SetAngle2(Angle2);
            Delay_ms(700);
            Angle1=150;
            Angle2=180;
            Servo_SetAngle1(Angle1);
            Servo_SetAngle2(Angle2);
            Delay_ms(700);
            Angle1=90;
            Angle2=90;
            Servo_SetAngle1(Angle1);
            Servo_SetAngle2(Angle2);
            Delay_ms(700);            
        }
        if(Key_GetNum()==2)//Ԥ������
        {
            
        }
    }
}	
