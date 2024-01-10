#ifndef _SERVO_H_
#define _SERVO_H

/*本头文件中使用STM32F103C8T6的PA^0~3引脚作为PWM输出口（具体对应请查看PWM.c文件），连接舵机信号线*/

void Servo_Init(void);
void Servo1_SetAngle(float Angle);
void Servo2_SetAngle(float Angle);
void Servo3_SetAngle(float Angle);
void Servo4_SetAngle(float Angle);   
//电机角度控制 输入量（0，180）
#endif
