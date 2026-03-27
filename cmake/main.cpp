#include <cstdlib>
#include <iostream>

#include "lib1.h"
#include "lib2.h"

int main() {
    std::cout << "start" << std::endl;
    lib1();
    lib2();
    std::cout << "complete" << std::endl;
    return EXIT_SUCCESS;
}
