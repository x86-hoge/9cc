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
        echo "$expected expected, but got $actual"
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
echo OK
