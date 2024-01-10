#include	"config.h"
#include	"GPIO.h"
#include	"delay.h"
#include    "OLED.h"


//void	GPIO_config(void)
//{
//	GPIO_InitTypeDef	GPIO_InitStructure;		//结构定义
//	GPIO_InitStructure.Pin  = GPIO_Pin_All;		//指定要初始化的IO, GPIO_Pin_0 ~ GPIO_Pin_7, 或操作
//	GPIO_InitStructure.Mode = GPIO_OUT_PP;		//指定IO的输入或输出方式,GPIO_PullUp,GPIO_HighZ,GPIO_OUT_OD,GPIO_OUT_PP
//	GPIO_Inilize(GPIO_P0,&GPIO_InitStructure);	//初始化
//}


void main()
{
    OLED_Init();
    OLED_ShowCN();
}
