#include	"config.h"
#include	"GPIO.h"
#include	"delay.h"
#include    "OLED.h"


//void	GPIO_config(void)
//{
//	GPIO_InitTypeDef	GPIO_InitStructure;		//�ṹ����
//	GPIO_InitStructure.Pin  = GPIO_Pin_All;		//ָ��Ҫ��ʼ����IO, GPIO_Pin_0 ~ GPIO_Pin_7, �����
//	GPIO_InitStructure.Mode = GPIO_OUT_PP;		//ָ��IO������������ʽ,GPIO_PullUp,GPIO_HighZ,GPIO_OUT_OD,GPIO_OUT_PP
//	GPIO_Inilize(GPIO_P0,&GPIO_InitStructure);	//��ʼ��
//}


void main()
{
    OLED_Init();
    OLED_ShowCN();
}
