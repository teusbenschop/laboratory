#!/bin/bash

rm -f main
if [ $? -ne 0 ]; then exit; fi
g++ -Wall -Wextra -pedantic -std=c++17 -lpthread -I/opt/local/include -L/opt/local/lib -lmbedtls -lmbedcrypto -lmbedx509 -o main main.cpp
if [ $? -ne 0 ]; then exit; fi
