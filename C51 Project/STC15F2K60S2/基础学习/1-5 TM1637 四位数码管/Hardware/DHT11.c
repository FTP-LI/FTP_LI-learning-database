#include <REG51.H>
#include "Delay.h"

sbit DHT = P1^3;

extern uint16_t datas [5];//8bitʪ����������+8bitʪ��С������+8bi�¶���������+8bit�¶�С������+8bitУ���

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
			while(!DHT);//��Ч���ݶ��Ǹߵ�ƽ
			delay_us(40);
			if(DHT == 1){
				flag=1; //����λ��1
				while(DHT);//����λ��1������70us,���Կ����ٽ��
			}else{
				flag = 0;//����λ��0
			}
			tmp = tmp << 1;//����һλ
			tmp |= flag; //��һλ����λ0��1����tmp
		}
		datas[i] = tmp;
	}
}
