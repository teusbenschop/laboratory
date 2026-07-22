#!/usr/bin/env bash

echo Compile first pass and may fail
g++ -std=c++23 -O3 -fmodules -fsearch-include-path bits/std.cc helloworld2.cpp module2.cppm -o helloworld2
echo Compile second pass should succeed
g++ -std=c++23 -O3 -fmodules -fsearch-include-path bits/std.cc helloworld2.cpp module2.cppm -o helloworld2
