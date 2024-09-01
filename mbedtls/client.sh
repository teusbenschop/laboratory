#!/bin/bash

find . -name client -delete
if [ $? -ne 0 ]; then exit 1; fi

echo Build client

g++ -Wall -Wextra -pedantic -std=c++17 -lpthread -o client client.cpp -I/opt/local/include -L/opt/local/lib -lmbedtls -lmbedcrypto -lmbedx509
#g++ -Wall -Wextra -pedantic -std=c++17 -lpthread -o client client.cpp mbedtls/*.o -I.
if [ $? -ne 0 ]; then exit 1; fi

