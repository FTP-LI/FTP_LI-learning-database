#ifndef __TM1637_H__
#define __TM1637_H__
 
 
// ��ʾ����
void TM1637_DisplayInt(unsigned int target);
 
// ��ʾС��: ʹ�� ':' �ָ�, ������λ��������λС��, �����ȡ
// void TM1637_DisplayFloat(unsigned float target);
 
// �Զ�����ʾ
void TM1637_Display(unsigned char a, unsigned char b, unsigned char c, unsigned char d, unsigned char h);
 
#endif