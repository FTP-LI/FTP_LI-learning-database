#include "PWM.h"

void PWM0_Init(void)//本函数为PWM_0模块初始化
{
    CMOD = 0x04;                //T0计时器控制PWM周期
    CCON = 0x00;                //不使用中断标志，CR完成后置一
    CCAPM0 = 0x42;              //使能比较功能，允许PWM输出
    CL = CH = 0;                //计数器初始化
    CCAP0L = CCAP0H = 0;        //比较值初始化
    PCA_PWM0 = 0x00;            //PWM_0:工作于八位PWM功能
    AUXR1 = 0xCC;               //使用PWM_0第一组PWM输出接口，即P1.1引脚
    
    CR = 1;//使能PWM模块
    Timer0_Init();
}

void Timer0_Init(void)		//11.0592MHz
{
	AUXR |= 0x80;			//定时器时钟1T模式
	TMOD &= 0xF0;			//设置定时器模式
	TL0 = 0xA0;				//设置定时初始值,864分频系数
	TH0 = 0xCF;				//设置定时初始值
	TF0 = 0;				//清除TF0标志
	TR0 = 1;				//定时器0开始计时
}

/*PWM频率计算
PWM=PCA时钟源/256=SYSclk/x/256 即系统时钟/分频系数/256
本函数中使用STC15F2K60S2 自带时钟源进行计算即11.0592MHz
可得要得到50Hz（即20ms）的PWM输出 分配系数为864，则T0计时器的初值为65536-864=64672 即0xFCA0
为了简便T0选择1T模式，16位自动重装，可通过STC-ISP快速得到代码
*/

void Steer_SetAngle(uint8_t Angle)
{
    CCAP0H = 249 - Angle/180*25;
}
/*PWM占空比计算
PWM占空比=(256-CCAPn)/256 本函数使用CCAP0模块固为CCAP0
对于舵机 PWM占空比2.5%~12.5%即0~180°
则CCAP0的范围为 249~224 编辑CCAP0H和CCAP0L寄存器
*/