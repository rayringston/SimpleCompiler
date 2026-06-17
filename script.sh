#!/bin/bash

g++ main.cpp -o compiler
./compiler test.txt
aarch64-linux-gnu-as out.s -o out.o
aarch64-linux-gnu-ld out.o -o out
