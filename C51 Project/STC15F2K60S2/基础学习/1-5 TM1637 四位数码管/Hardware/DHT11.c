#include <REG51.H>
#include "Delay.h"

sbit DHT = P1^3;

extern uint16_t datas [5];//8bit湿度整数数据+8bit湿度小数数据+8bi温度整数数据+8bit温度小数数据+8bit校验和

void DHT11_start(void)
{
	DHT=1;
	DHT=0;
	delay_ms(30);
	DHT = 1;
	while(DHT);
	while(!DHT);
	while(DHT);
}


void Read_Data_From_DHT(void)
{
	int i;
	int j;
	char tmp;
	char flag;
	DHT11_start();
	for(i=0;i<5;i++){
		for(j=0;j<8;j++){
			while(!DHT);//有效数据都是高电平
			delay_us(40);
			if(DHT == 1){
				flag=1; //数据位是1
				while(DHT);//数据位是1，持续70us,所以卡个临界点
			}else{
				flag = 0;//数据位是0
			}
			tmp = tmp << 1;//左移一位
			tmp |= flag; //将一位数据位0或1存入tmp
		}
		datas[i] = tmp;
	}
}
