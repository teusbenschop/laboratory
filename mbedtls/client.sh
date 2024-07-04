#!/bin/bash

rm -f /tmp/client

g++ -Wall -Wextra -pedantic -std=c++17 -lpthread -I/opt/local/include -L/opt/local/lib -lmbedtls -lmbedcrypto -lmbedx509 -o /tmp/client client.cpp
if [ $? -ne 0 ]; then exit; fi

/tmp/client
