#!/bin/bash

rm *.yy.c *.tab.* *.o main > /dev/null 2>&1

flex lex.l
bison -d parser.y 

gcc -c lex.yy.c -o lex.o
gcc -c parser.tab.c -o parser.o
gcc -c main.c -o main.o

gcc main.o parser.o lex.o -o main -I.
