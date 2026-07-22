module;

#include <print>
#include <stdio.h>

export module demo;

import std;

export void func(std::string s) {
    std::println("s = {}", s);
}

export template <class X>
class A {
public:
    void f(X x) { printf("old C...\n"); }
};
