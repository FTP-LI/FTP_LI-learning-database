#ifndef __TM1637_H__
#define __TM1637_H__
 
 
// 显示整数
void TM1637_DisplayInt(unsigned int target);
 
// 显示小数: 使用 ':' 分隔, 仅限两位整数和两位小数, 否则截取
// void TM1637_DisplayFloat(unsigned float target);
 
// 自定义显示
void TM1637_Display(unsigned char a, unsigned char b, unsigned char c, unsigned char d, unsigned char h);
 
#endif