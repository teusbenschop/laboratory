#!/bin/bash

find . -name server -delete
if [ $? -ne 0 ]; then exit 1; fi

echo Build server
g++ -Wall -Wextra -pedantic -std=c++17 -lpthread -o server server.cpp -I/opt/local/include -L/opt/local/lib -lmbedtls -lmbedcrypto -lmbedx509
if [ $? -ne 0 ]; then exit 1; fi
