#include<stdio.h>
#include <stdlib.h>

void echo(int num){
    printf("関数呼び出しに成功\n 引数値%d\n",num);
    //return num;
}
// void alloc4(int *p,int a,int b,int c,int d){
//     printf("引数：%d %d %d %d\n",a,b,c,d);
//     printf("%d %d %d\n",&p,sizeof(p),p);
//     int *e= malloc(sizeof(int)*4);
//     p = e;
//     p[0]=a;p[1]=b;p[2]=c;p[3]=d;
//     printf("%d %d %d %d %d\n",&p,p[0],p[1],p[2],e[3]);
//     printf("取得値：%d\n",*p);
    
// }
int* alloc4(int a,int b,int c,int d){
    printf("引数：%d %d %d %d\n",a,b,c,d);
    int *p= malloc(sizeof(int)*4);
    p[0]=a;p[1]=b;p[2]=c;p[3]=d;
    return p;
}
void alloc3(int **p,int a,int b,int c){
    printf("引数：%d %d %d\n",a,b,c);
    *p = malloc(sizeof(int)*4);
    **p=a;
    *(*p+1)=b;
    *(*p+2)=c;
}

void check_addr(int *p){
    printf("%d\n",*p);
}

void check_ptr(int *p){
    *p = 113;
}