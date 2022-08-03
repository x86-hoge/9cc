#include<stdio.h>
#include <stdlib.h>

void echo(int num){
    printf("関数呼び出しに成功\n 引数値%d\n",num);
    //return num;
}

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

void outok(char *s,int a){
    printf("OK:%s =>  %d\n",s,a);
    exit(1);
}
void outerr(char *s,int a){
    printf("ERROR:%s =>  %d\n",s,a);
    exit(1);
}