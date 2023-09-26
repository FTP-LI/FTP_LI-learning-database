#include "stm32f10x.h"                  // Device header
#include "delay.h"
#include "OLED.h"
#include "Servo.h"
#include "key.h"

int8_t KeyNum;//8位全局变量
	
int main(void)
{
    float Angle1,Angle2;
    int i,j;
	OLED_Init();
	Servo_Init();
	Key_Init();
	
	Servo_SetAngle1(90);//上层电机，值减小向后
    Servo_SetAngle2(90);//下层电机，值减小向左
	//电机初始状态
	OLED_ShowString( 1 , 1 , "Angle1:" );
    OLED_ShowString( 2 , 1 , "Angle2:" );
	
	
	while(1)
	{
        if(Key_GetNum()==1)//预设程序一
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
        if(Key_GetNum()==2)//预设程序二
        {
            
        }
    }
}	
