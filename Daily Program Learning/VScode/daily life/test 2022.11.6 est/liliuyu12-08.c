#include<stdio.h>
int main()
{
	float x,y;
	printf("������˿͹����ܽ�");
	scanf("%f",&x); 
	if(x<=200&&x>0){
		y=0.85*x;
	}
	else if(x>200&&x<=500){
		y=0.8*x-6;
	}
	else if(x>500){
		y=0.75*x-9;
	}
	else {
	printf("���ݴ���");
	}
	printf("�˿�Ӧ����%.2f",y);
	return 0;
}
