#include<stdio.h>
#include <stdlib.h>

void echo(int num){
    printf("関数呼び出しに成功\n 引数値%d\n",num);
    //return num;
}
void alloc4(int *p,int a,int b,int c,int d){
    printf("引数：%d %d %d %d\n",a,b,c,d);
    printf("%d %d %d\n",&p,sizeof(p),p);

    p = malloc(sizeof(int)*4);
    p[0]=a;p[1]=b;p[2]=c;p[3]=d;
    printf("%d %d %d %d %d\n",&p,p[0],p[1],p[2],p[3]);
    printf("取得値：%d\n",*p);
    
}
void check_addr(int *p){
    printf("%d\n",*p);
}
void check_ptr(int *p){
    *p = 113;
}