#include<stdio.h>
#include<string.h>
void fun (char a[],char b[],char c[]);
int main()
{
	char a[20];
	char b[20];
	char c[40];
	printf("�����һ���ַ���:\n");
	gets(a);
	printf("����ڶ����ַ���:\n"); 
	gets(b);
	fun(a,b,c);
} 
void fun (char a[],char b[],char c[])
{
	printf("ƴ�ӵ��ַ���Ϊ:\n%s",strcat(a,b)); 
}
