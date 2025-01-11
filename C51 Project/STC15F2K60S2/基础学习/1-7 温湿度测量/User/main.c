#include <STC15F2K60S2.H>
#include "Delay.h"
#include "F2481.h"
#include "DHT11.h"

extern uint8_t HmdHighDec,TmpHighDec,HmdLowDec,TmpLowDec;
uint8_t HH,HM,TH,TM,FLAG=0,time=0;      //对应三位湿度显示和三位温度显示的高中低三位

void datas_cout(void)
{
    DHT11_Receive();            //DHT11数据接收
    Tran_Dec();                 //数据转换
    HH=HmdHighDec/10;
    HM=HmdHighDec%10;
    TH=TmpHighDec/10;
    TM=TmpHighDec%10;
}

void INT0_Init(void){
  IT0 = 1;   //只允许下降沿触发
  EX0 = 1;    //中断外部中断0中断
  EA = 1;     //允许存在中断
}

void INT0_handle(void) interrupt 0
    {
        FLAG++;
        FLAG = FLAG%2;
        time = 0;
        delay_ms(10);
}

void main()
{
    uint8_t N=0;//构建状态机
    uint32_t TIM=0;//软件定时
    datas_cout();                   //初始数据获取
    INT0_Init();
    while(1)
    {
        TIM ++;
        time ++;
        if(time == 3500)
        {
            time = 0;
            FLAG++;
            FLAG = FLAG%2;
        }
        if(TIM == 600)
        {
            datas_cout();
            TIM = 0;
        }
        if(FLAG == 0)//温度
        {
            F24811_display(2,TH);
            delay_ms(1);
            F24811_display(3,TM);
            delay_ms(1);
            F24811_display(4,16);
            delay_ms(1);
        }
        if(FLAG == 1)//湿度
        {
            F24811_display(2,HH);
            delay_ms(1);
            F24811_display(3,HM);
            delay_ms(1);
            F24811_display(4,12);
            delay_ms(1);
        }
    }
}
