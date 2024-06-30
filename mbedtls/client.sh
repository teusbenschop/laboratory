#!/bin/bash

g++ -Wall -Wextra -pedantic -std=c++17 -lpthread -I/opt/local/include -L/opt/local/lib -lmbedtls -lmbedcrypto -lmbedx509 -o client client.cpp
if [ $? -ne 0 ]; then exit; fi

./client
if [ $? -ne 0 ]; then exit; fi

rm client
