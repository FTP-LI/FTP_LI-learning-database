#ifndef _SERVO_H_
#define _SERVO_H

/*��ͷ�ļ���ʹ��STM32F103C8T6��PA^0~3������ΪPWM����ڣ������Ӧ��鿴PWM.c�ļ��������Ӷ���ź���*/

void Servo_Init(void);
void Servo1_SetAngle(float Angle);
void Servo2_SetAngle(float Angle);
void Servo3_SetAngle(float Angle);
void Servo4_SetAngle(float Angle);   
//����Ƕȿ��� ��������0��180��
#endif
