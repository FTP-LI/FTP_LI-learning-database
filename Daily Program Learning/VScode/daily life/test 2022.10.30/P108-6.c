#include<stdio.h>
int main()
{
    int x,y;
    printf("请输入一个数字：");
    scanf("%d",&x);
    if(x<1) y=x;
    else if(x>=1&&x<10) y=2*x-1;
    else y=3*x-11;
    printf("计算得：%d",y);
    return 0;
}