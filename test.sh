#!/bin/bash

try(){
	expected="$1"
	input="$2"

	./9cc "$input" > tmp.s
	gcc -o tmp tmp.s
	./tmp
	actual="$?"

	if [ "$actual" = "$expected" ]; then
		echo "$input => $actual"
	else
		echo "$expected expected, but got $actual"
		exit 1
	fi
}

try 0  "main(){0;                               }" 
try 42 "main(){42;                              }"
try 21 "main(){5+20-4;                          }"
try 41 "main(){12 + 34 - 5;                     }"
try 47 "main(){5+6*7;                           }"
try 15 "main(){5*(9-6);                         }"
try 4  "main(){(3+5)/2;                         }"
try 2  "main(){1+1;1+1;                         }"
try 1  "main(){a=1;                             }"
try 1  "main(){a=b=1;                           }"
try 1  "main(){20==20;                          }"
try 1  "main(){20!=12;                          }"
try 1  "main(){10==(5+5);                       }"
try 0  "main(){1==2;                            }"
try 10 "main(){a=1;b=9;a+b;                     }"
try 20 "main(){val=3;test=2;num=15;val+test+num;}"
try 15 "main(){sd=10;sd=sd+10/2; return sd;}"
echo OK
