#include <stdio.h>

int main()
{
    char n='A';
    int i,j,k;
    for(i=1;i<5;i++)//主循环
    {
        for(k=0;k<5-i;k++)
            {
                printf(" ");
            }//输出空格
        for(j=0;j<2*i-1;j++)
        {
            printf("%c",n);
        }//输出字母
        n++;//字母迭代
        printf("\n");//换行
    }
}