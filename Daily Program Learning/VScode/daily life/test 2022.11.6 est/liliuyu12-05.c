#include<stdio.h>
int main()
{
    int a=0,b=0,c=0,d=0;//a��ʾӢ����ĸ,b��ʾ�ո�,c��ʾ����,d��ʾ�����ַ� 
    char e;
    printf("����һ���ַ���ȷ��������ɳɷ�:");
    while((e=getchar())!='\n')
    {
        if(e>='A'&&e<='Z')
        a++;
        else if(e>='a'&&e<='z')
        a++;
        else if(e==' ') 
        b++;
        else if(e>='0'&&e<='9')
        c++;
        else 
        d++;
    }
    printf("Ӣ����ĸ����Ϊ:%d �ո����Ϊ:%d ���ָ���Ϊ:%d �����ַ�����Ϊ:%d",a,b,c,d);
    return 0;
}
