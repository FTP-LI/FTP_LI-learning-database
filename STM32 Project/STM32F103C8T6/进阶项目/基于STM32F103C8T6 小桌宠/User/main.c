#include "stm32f10x.h"                  // Device header
#include "final.h"

/*
———基于STM32F103C8T6小桌宠
———版本V1.1-2023/12/22
———硬件：OLED、LED、NRF24L01、STM32F103C8T6、SG90舵机、微动开关
———总结：此版本为初始版本，存在较多问题，作者将在后期持续改良
———声明：非作者允许，不能用于商业用途，本代码完全开源
*/

int main(void)
{
    Final_Init();//总初始化
    while(1)
    {
        crol();
    }
}
