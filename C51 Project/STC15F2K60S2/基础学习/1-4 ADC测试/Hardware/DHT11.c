#include <REG51.H>
#include "Delay.h"

sbit Data=P1^6;
sbit DHT11=P1^6;

uint8_t rec_dat[9]; 

/*使用规则
//uint8_t wendu [4];
//uint8_t sidu [4];
//for(i=0;i<7;i++) 
//    {
//        if(i<=2)
//            {
//                sidu[i]= rec_dat[i]; //读取湿度
//			}
//            else
//			{
//				wendu[i-4] = rec_dat[i];//读取温度
//			}	   
//    }
//    OLED_ShowString(1,1,sidu)//进行显示
*/

void DHT11_start(void)
{
   DHT11=1;
   delay_us(2);
   DHT11=0;
   delay_ms(30);   
   DHT11=1;
   delay_us(30);
}//请求数据

uint8_t DHT11_rec_byte(void)      
{
   uint8_t i,dat=0;
  for(i=0;i<8;i++)    
   {          
      while(!DHT11);   
      delay_us(8);    
      dat<<=1;           
      if(DHT11==1)    
         dat+=1;
      while(DHT11);     
    }  
    return dat;
}

void DHT11_receive(void)     
{
    uint8_t R_H,R_L,T_H,T_L,RH,RL,TH,TL,revise; 
    DHT11_start();
    if(DHT11==0)
    {
        while(DHT11==0);       
        delay_us(40);  
        R_H=DHT11_rec_byte();      
        R_L=DHT11_rec_byte();     
        T_H=DHT11_rec_byte();     
        T_L=DHT11_rec_byte();    
        revise=DHT11_rec_byte(); 
        delay_us(25);    
        if((R_H+R_L+T_H+T_L)==revise)      
        {
            RH=R_H;
            RL=R_L;
            TH=T_H;
            TL=T_L;
        }  
        rec_dat[0]='0'+(RH/10);
        rec_dat[1]='0'+(RH%10);
				rec_dat[2]=' ';
				rec_dat[3]=' ';
				//温度
        rec_dat[4]='0'+(TH/10);
        rec_dat[5]='0'+(TH%10); 
		rec_dat[6]=' ';
    }
}