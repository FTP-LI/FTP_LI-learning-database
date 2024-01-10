#include <STC15F2K60S2.H>
#include "OLED.h"
#include "Delay.h"
#include "TM1637.h"
#include "KEY.h"
#include "DHT11.h"

extern uint8_t HmdHighDec,TmpHighDec,HmdLowDec,TmpLowDec;
uint8_t HH,HM,HL,TH,TM,TL;      //对应三位湿度显示和三位温度显示的高中低三位

void datas_cout(void)
{
    DHT11_Receive();            //DHT11数据接收
    Tran_Dec();                 //数据转换
    HH=HmdHighDec/10;
    HM=HmdHighDec%10;
    HL=HmdLowDec/10;
    TH=TmpHighDec/10;
    TM=TmpHighDec%10;
    TL=TmpLowDec/10;            //显示数据处理
}

void main()
{
    uint8_t KEY;//按键标志位
    uint8_t N=0;//构建状态机
    uint32_t time=0,TIM=0;//软件定时
    datas_cout();                   //初始数据获取
    while(1)
    {
        KEY=key_scan();//按键监测
        if(KEY==1)
        {
            N++;                        //状态轮转
            N=N%2;                      //对2取余保证两种状态即N=0和N=1
            delay_ms(500);
        }
        if(N==0)//温度显示模式
        {
            TM1637_Display(TH,TM,TL,12,1);
        }
        else if(N==1)//湿度显示模式
        {
            TM1637_Display(HH,HM,HL,16,1);
        }
        time++;
        if(time==2090)
        {
            time=0;
            TIM++;
            if(TIM==10000)
            {
               datas_cout();//数据刷新
            }
        }//软件定时效果，间隔大概10s误差5%
    }
}
