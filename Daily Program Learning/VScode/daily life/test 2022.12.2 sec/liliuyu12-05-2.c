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
	int x,y;
	strcpy(c,a);
	for(x=strlen(a),y=0;b[y]!=0;x++,y++){
	    c[x]=b[y]; 
	}
	c[x]='\0';
	printf("ƴ�Ӻ���ַ���Ϊ:\n"); 
	puts(c);
}
