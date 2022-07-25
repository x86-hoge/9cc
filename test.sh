#!/bin/bash

try(){
    expected="$1"
    input="$2"

    ./9cc "$input" > tmp.s
    gcc -o tmp tmp.s sample.o
    ./tmp
    actual="$?"

    if [ "$actual" = "$expected" ]; then
        echo "$input => $actual"
    else
        echo "結果は'$expected 'ではありませんでした。取得した値 '$actual'"
        exit 1
    fi
}

try 0   "int main(){0;}" 
try 42  "int main(){42;}"
try 21  "int main(){5+20-4;}"
try 41  "int main(){12 + 34 - 5;}"
try 47  "int main(){5+6*7;}"
try 15  "int main(){5*(9-6);}"
try 4   "int main(){(3+5)/2;}"
try 2   "int main(){1+1;1+1;}"
try 1   "int main(){int a; a=1;}"
try 1   "int main(){int a;int b;a=b=1;}"
try 1   "int main(){20==20;}"
try 1   "int main(){20!=12;}"
try 1   "int main(){10==(5+5);}"
try 0   "int main(){1==2;}"
try 10  "int main(){int a;int b;a=1;b=9;a+b;}"
try 20  "int main(){int val;int test;int num;val=3;test=2;num=15;val+test+num;}"
try 15  "int main(){int sd;sd=10;sd=sd+10/2; return sd;}"
try 10  "int main(){int a;a=20;if(1==1){10;}}"
try 1   "int main(){if(1==1){5;}else{1;}if(1==0){1;4+1;}else{100-99;}}"
try 10  "int main(){int a;a=1;while(a!=10){a=a+1;}}"
try 60  "int main(){int a;a=1;while(a!=10){a=a+1;}a+50;}"
try 30  "int main(){int a;a=29;if(a!=1){a=a+1;}a;}"
try 10  "int main(){for(int i=5; i!=10; i=i+1){12;}i;}"
try 55  "int main(){int a;a=0;for(int i=1; i!=11; i=i+1){a=a+i;}return a;}"
try 46  "int main(){int a;a=0;for(int i=1; i!=11; i=i+1){if(a!=45){a=a+i;}else{a=a+1;}}a;}"
try 10  "int main(){int a;a=1;for(int i=5; i!=10; i=i+1){echo(i);}i;}"
try 5   "int main(){for(int i=1; i<5; i=i+1){echo(i);}i;}"
try 6   "int main(){for(int i=1; i<=5; i=i+1){echo(i);}i;}"
try 1   "int main(){int returna;int ifa;int elsea;int fora;returna=1;ifa=1;elsea=1;fora=1;}"
try 255 "int main(){-1;}"
try 20  "int main(){int a;int b;a=10;b=&a;return *b*2;}"
try 100  "int main(){int a;int *b;b=&a;*b=100;return a;}"
try 3    "int main(){int x;int *y;y=&x;*y=3;return x;}"
try 6   "int main(){go();6;} int go(){echo(10);}"
try 232 "int main(){go();} int go(){echo(10);return 232;}"
try 6   "int main(){go(10);return 6;} int go(int rea){echo(rea);}"
try 12  "int main(){go(40,50,60,70,80,90);12;}int go(int a,int b,int c,int d,int e,int f){z(a);z(b);z(c);z(d);z(e);z(f); return 12;}int z(int grand){echo(grand);}"
try 0   "int main(){int p;p=10;check_addr(&p);0;}"
try 113 "int main(){int *p;int a;a=10;p=&a;check_ptr(&p);p;}"
try 241 "int main(){int *p;int a;int b;int c;a=10;b=11;c=241;p=&a;p=p-4;*p;}"
try 12  "int main(){int b;b=get_num();b;} int get_num(){return 12;}"
try 1   "int main(){int *p;p=alloc4(1, 2, 4, 8);*p;}"
try 2   "int main(){int *p;p=alloc4(1, 2, 4, 8);p=p+1;*p;}"
try 4   "int main(){int *p;p=alloc4(1, 2, 4, 8);p=p+2;*p;}"
try 2   "int main(){int *p;alloc3(&p, 2, 4, 8);*p;}"
try 4   "int main(){int *p;alloc3(&p, 2, 4, 8);p=p+1;*p;}"
try 8   "int main(){int *p;alloc3(&p, 2, 4, 8);p=p+2;*p;}"
try 4   "int main(){sizeof int;sizeof(int);}"
try 8   "int main(){sizeof int*;sizeof(int*);}"

echo OK
