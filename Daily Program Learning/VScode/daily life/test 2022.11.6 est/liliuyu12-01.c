#include<stdio.h>
int main()
{
    int a,b,c,d;
    printf("����������a,b,c�����ǴӴ�С����");
    scanf("%d,%d,%d",&a,&b,&c);
    if(a<b){
        d=a,a=b,b=d;
    }
    if(a<c){
        d=a,a=c,c=d;
    }
    if(b<c){
        d=b,b=c,c=d;
    }
    printf("�Ӵ�С����Ϊ��%d>%d>%d",a,b,c);
    return 0;
}
