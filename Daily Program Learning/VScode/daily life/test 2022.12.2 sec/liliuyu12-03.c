#include <stdio.h>
void fun(int c[], int a[], int b[]);
int main()
 {
 int a[10]={0}; //a��ʾ��������
 int b[10]={0}; //b��ʾż������
 int c[10];
 int n;
 printf("����10������:");
 for (n = 0; n < 10; n++)
  scanf("%d", &c[n]);
 fun(c,a,b);
 printf("������:") ;
 for (n=0;a[n]!= 0;n++)
  printf("%d ", a[n]);
 printf("\nż����:");
 for (n = 0; b[n] != 0; n++)
  printf("%d ", b[n]);
 return 0;
}
void fun(int c[], int a[], int b[]) 
{
 int x=0,y=0,z = 0;
 for (x=0;x<10;x++)
  {
    if (c[x]%2== 0)
     {
       b[y++] = c[x];
     }
    else 
     {
       a[z++] = c[x];
     }
   }
}
