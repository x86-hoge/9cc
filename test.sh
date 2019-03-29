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

try 0  "0;" 
try 42 "42;"
try 21 "5+20-4;"
try 41 "12 + 34 - 5;"
try 47 "5+6*7;"
try 15 "5*(9-6);"
try 4  "(3+5)/2;"
try 2  "1+1;1+1;"
try 1  "a=1;"
try 1  "a=b=1;"
try 1  "20==20;"
try 1  "20!=12;"
try 1  "10==(5+5);"
try 0  "1==2;"
try 10 "a=1;b=9;a+b;"
echo OK
