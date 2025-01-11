#include <STC15F2K60S2.H>
#include "Delay.h"
#include "Sys.h"
#include "F2481.h"

sbit F2481_1=P1^1;	
sbit F2481_2=P1^2;	
sbit F2481_3=P1^3;	
sbit F2481_4=P1^4;	
//Î»ºÅ
sbit F2481_A=P0^0;	
sbit F2481_B=P0^1;	
sbit F2481_C=P0^2;	
sbit F2481_D=P0^3;	
sbit F2481_E=P0^4;	
sbit F2481_F=P0^5;	
sbit F2481_G=P0^6;	
sbit F2481_DP=P0^7;
//ÊýÂë¹Ü

code unsigned char num[] =
{
        0x3F,/*0*/
        0x06,/*1*/
        0x5B,/*2*/
        0x4F,/*3*/
        0x66,/*4*/
        0x6D,/*5*/
        0x7D,/*6*/
        0x07,/*7*/
        0x7F,/*8*/
        0x6F,/*9*/
        0x77,/*10 A*/
        0x7C,/*11 b*/
        0x39,/*12 c*/
        0x5E,/*13 d*/
        0x79,/*14 E*/
        0x71,/*15 F*/
        0x76,/*16 H*/
        0x38,/*17 L*/
        0x54,/*18 n*/
        0x73,/*19 P*/
        0x3E,/*20 U*/
};

void F24811_display(uint8_t weishu,uint8_t DATA)
{
    if(weishu == 1)
    {
        F2481_1 = 0;
        F2481_2 = 1;
        F2481_3 = 1;
        F2481_4 = 1;
    }
    if(weishu == 2)
    {
        F2481_1 = 1;
        F2481_2 = 0;
        F2481_3 = 1;
        F2481_4 = 1;
    }
    if(weishu == 3)
    {
        F2481_1 = 1;
        F2481_2 = 1;
        F2481_3 = 0;
        F2481_4 = 1;
    }
    if(weishu == 4)
    {
        F2481_1 = 1;
        F2481_2 = 1;
        F2481_3 = 1;
        F2481_4 = 0;
    }
    P0 = ~num[DATA];
}

void F24811_freshall(uint16_t number)
{
    uint16_t a,b,c,d;
    a = number/1000;
    b = number/100%10;
    c = number/10%10;
    d = number%10;
    F24811_display(1,a);
    delay_ms(5);
    F24811_display(2,b);
    delay_ms(5);
    F24811_display(3,c);
    delay_ms(5);
    F24811_display(4,d);
    delay_ms(5);
}