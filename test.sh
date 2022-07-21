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

try 0  "main(){0;}" 
try 42 "main(){42;}"
try 21 "main(){5+20-4;}"
try 41 "main(){12 + 34 - 5;}"
try 47 "main(){5+6*7;}"
try 15 "main(){5*(9-6);}"
try 4  "main(){(3+5)/2;}"
try 2  "main(){1+1;1+1;}"
try 1  "main(){a=1;}"
try 1  "main(){a=b=1;}"
try 1  "main(){20==20;}"
try 1  "main(){20!=12;}"
try 1  "main(){10==(5+5);}"
try 0  "main(){1==2;}"
try 10 "main(){a=1;b=9;a+b;}"
try 20 "main(){val=3;test=2;num=15;val+test+num;}"
try 15 "main(){sd=10;sd=sd+10/2; return sd;}"
try 10 "main(){a=20;if(1==1){10;}}"
try 1  "main(){if(1==1){5;}else{1;}if(1==0){1;4+1;}else{100-99;}}"
try 10 "main(){a=1;while(a!=10){a=a+1;}}"
try 60 "main(){a=1;while(a!=10){a=a+1;}a+50;}"
try 30 "main(){a=29;if(a!=1){a=a+1;}a;}"
try 10 "main(){for(i=5; i!=10; i=i+1){12;}i;}"
try 55 "main(){a=0;for(i=1; i!=11; i=i+1){a=a+i;}return a;}"
try 46 "main(){a=0;for(i=1; i!=11; i=i+1){if(a!=45){a=a+i;}else{a=a+1;}}a;}"
try 10 "main(){for(i=5; i!=10; i=i+1){echo(i);}i;}"
echo OK
