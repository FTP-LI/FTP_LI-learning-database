#include<stdio.h>
int main()
{
	int a,b,c,d;//a��b��c�ֱ��ʾ�ꡢ�¡��ա�d��ʾ���� 
	printf("������,��,��");
	scanf("%d,%d,%d",&a,&b,&c);
	if((a%400==0)||((a%4==0)&&(a%100!=0)))
	{
	printf("��������\n");
	switch(b)
	{
	case 1:d=0;break;
	case 2:d=31;break;
	case 3:d=60;break;
	case 4:d=91;break;
	case 5:d=121;break;
	case 6:d=152;break;
	case 7:d=182;break;
	case 8:d=213;break;
	case 9:d=244;break;
	case 10:d=274;break;
	case 11:d=305;break;
	case 12:d=335;break;
	default:printf("�����������\n"); 
    }
	}
	else
	printf("����ƽ��\n");
	switch(b)
	{
	case 1:d=0;break;
	case 2:d=31;break;
	case 3:d=59;break;
	case 4:d=90;break;
	case 5:d=120;break;
	case 6:d=151;break;
	case 7:d=181;break;
	case 8:d=212;break;
	case 9:d=243;break;
	case 10:d=273;break;
	case 11:d=304;break;
	case 12:d=334;break;
	default:printf("�����������\n"); 
	}
	d+=c; 
	printf("��һ������һ��ĵ�%d��\n",d);
	return 0;
}
