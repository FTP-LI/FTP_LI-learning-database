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



//ALIENTEK Mini STM32�����巶������30
//������ʾ ʵ��  
//����֧�֣�www.openedv.com
//������������ӿƼ����޹�˾  
 
extern unsigned int rec_data[4]; 
 
 
void BUZZER_Init(void)
{
 
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //ʹ��PA,PD�˿�ʱ��
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;				 //LED0-->PA.8 �˿�����
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
 GPIO_Init(GPIOA, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOA.8
    GPIO_ResetBits(GPIOA,GPIO_Pin_0);
    
} 
 
 int main(void)
 { 
	u8 key;
  
     
     NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);// �����ж����ȼ�����2 
	delay_init();	    	 //��ʱ������ʼ��	  
	uart_init(9600);	 	//���ڳ�ʼ��Ϊ9600			 
	LCD_Init();			//��ʼ��Һ�� 
	LED_Init();         //LED��ʼ��	 													    
	KEY_Init();				//������ʼ��	  													    
	usmart_dev.init(72);	//usmart��ʼ��	
 	mem_init();				//��ʼ���ڴ�� 
    BUZZER_Init();     
     exfuns_init();			//Ϊfatfs��ر��������ڴ�  
    f_mount(fs[0],"0:",1); 	//����SD�� 
 	f_mount(fs[1],"1:",1); 	//����FLASH.
	while(font_init()) 	//����ֿ�
	{ 
		LCD_Clear(WHITE);		   	//����
 		POINT_COLOR=RED;			//��������Ϊ��ɫ	   	   	  
		while(SD_Initialize())		//���SD��
		{
			delay_ms(200);
			LCD_Fill(60,70,200+60,70+16,WHITE);
			delay_ms(200);		    
		}								 						    
	 	key=update_font(20,110,16);//�����ֿ�
		while(key)//����ʧ��		
		{			 		  
			delay_ms(200);
			LCD_Fill(20,110,200+20,110+16,WHITE);
			delay_ms(200);		       
		} 		  
		delay_ms(1500);	
		LCD_Clear(WHITE);//����	       
	}
	POINT_COLOR=BLACK;       
	Show_Str(0,30,200,24,"ͨ��20-2",24,0);
    Show_Str(0,60,200,24,"���� 2006030208",24,0);
    Show_Str(0,100,200,24,"ͨ��20-3",24,0);
    Show_Str(0,130,200,24,"���2006030321",24,0); 
    
    POINT_COLOR=RED;
    Show_Str(0,170,80,24,"�¶ȣ�",24,0);
    Show_Str(0,200,80,24,"ʪ�ȣ�",24,0);
    
    POINT_COLOR=BLUE;
    
    while(1)
    {
    DHT11_REC_Data(); //�����¶Ⱥ�ʪ�ȵ�����
		
        LCD_ShowxNum(60,170,rec_data[2],2,24,0);//�¶�
        Show_Str(90,170,24,24,".",24,0);
        LCD_ShowxNum(100,170,rec_data[3],2,24,0);
        Show_Str(130,170,40,24,"��C",24,0);
        LCD_ShowxNum(60,200,rec_data[0],2,24,0);//ʪ��
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

