g++ -std=c++20 -Wall -Wno-unknown-pragmas -Wextra -pedantic  -g -O2 -I/opt/local/include -L/opt/local/lib -lmicrohttpd *.cpp
if [ $? -ne 0 ]; then exit; fi
./a.out
rm -rf a.out*
