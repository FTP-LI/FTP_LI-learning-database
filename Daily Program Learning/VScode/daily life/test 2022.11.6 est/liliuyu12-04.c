#include<stdio.h>
int main()
{
    int m,n,i,a,b;
    printf("������������a,b��������ǵ����Լ������С������:");
    scanf("%d,%d",&m,&n);
    if(m>n)
    m=m,n=n;
    else
    i=m,m=n,n=i;
    for(a=m;a>=1;a--)
        if((m%a==0)&&(n%a==0))
        {
         printf("���Լ��Ϊ:%d\n",a);
         break;
        }
    b=m*n/a;//��ʽ������С������ 
    printf("��С������:%d\n",b);
    return 0;
}
