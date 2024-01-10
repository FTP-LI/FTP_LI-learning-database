#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "lcd.h"
#include "key.h"
#include "usmart.h" 
#include "malloc.h"  
#include "MMC_SD.h" 
#include "ff.h"  
#include "exfuns.h"
#include "fontupd.h"
#include "text.h"
#include "DHT11.h"



//ALIENTEK Mini STM32开发板范例代码30
//汉字显示 实验  
//技术支持：www.openedv.com
//广州市星翼电子科技有限公司  
 
extern unsigned int rec_data[4]; 
 
 
void BUZZER_Init(void)
{
 
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //使能PA,PD端口时钟
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;				 //LED0-->PA.8 端口配置
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
 GPIO_Init(GPIOA, &GPIO_InitStructure);					 //根据设定参数初始化GPIOA.8
    GPIO_ResetBits(GPIOA,GPIO_Pin_0);
    
} 
 
 int main(void)
 { 
	u8 key;
  
     
     NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);// 设置中断优先级分组2 
	delay_init();	    	 //延时函数初始化	  
	uart_init(9600);	 	//串口初始化为9600			 
	LCD_Init();			//初始化液晶 
	LED_Init();         //LED初始化	 													    
	KEY_Init();				//按键初始化	  													    
	usmart_dev.init(72);	//usmart初始化	
 	mem_init();				//初始化内存池 
    BUZZER_Init();     
     exfuns_init();			//为fatfs相关变量申请内存  
    f_mount(fs[0],"0:",1); 	//挂载SD卡 
 	f_mount(fs[1],"1:",1); 	//挂载FLASH.
	while(font_init()) 	//检查字库
	{ 
		LCD_Clear(WHITE);		   	//清屏
 		POINT_COLOR=RED;			//设置字体为红色	   	   	  
		while(SD_Initialize())		//检测SD卡
		{
			delay_ms(200);
			LCD_Fill(60,70,200+60,70+16,WHITE);
			delay_ms(200);		    
		}								 						    
	 	key=update_font(20,110,16);//更新字库
		while(key)//更新失败		
		{			 		  
			delay_ms(200);
			LCD_Fill(20,110,200+20,110+16,WHITE);
			delay_ms(200);		       
		} 		  
		delay_ms(1500);	
		LCD_Clear(WHITE);//清屏	       
	}
	POINT_COLOR=BLACK;       
	Show_Str(0,30,200,24,"通信20-2",24,0);
    Show_Str(0,60,200,24,"李轲 2006030208",24,0);
    Show_Str(0,100,200,24,"通信20-3",24,0);
    Show_Str(0,130,200,24,"杨博文2006030321",24,0); 
    
    POINT_COLOR=RED;
    Show_Str(0,170,80,24,"温度：",24,0);
    Show_Str(0,200,80,24,"湿度：",24,0);
    
    POINT_COLOR=BLUE;
    
    while(1)
    {
    DHT11_REC_Data(); //接收温度和湿度的数据
		
        LCD_ShowxNum(60,170,rec_data[2],2,24,0);//温度
        Show_Str(90,170,24,24,".",24,0);
        LCD_ShowxNum(100,170,rec_data[3],2,24,0);
        Show_Str(130,170,40,24,"°C",24,0);
        LCD_ShowxNum(60,200,rec_data[0],2,24,0);//湿度
        Show_Str(90,200,24,24,".",24,0);
        LCD_ShowxNum(100,200,rec_data[1],2,24,0);
        Show_Str(130,200,24,24,"%",24,0);
        delay_ms(500);
        if(rec_data[2]>24&&rec_data[2]<50)
        {
               GPIO_WriteBit(GPIOA,GPIO_Pin_0,Bit_SET); 
        }
        else{
            GPIO_WriteBit(GPIOA,GPIO_Pin_0,Bit_RESET);
        }
    }        
}

