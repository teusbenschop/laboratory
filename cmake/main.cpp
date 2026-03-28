#include <cstdlib>
#include <iostream>
#include <ostream>

#include "version.h"
#include "lib1.h"
#include "lib2.h"

int main() {
    std::cout << VERSION_MAJOR << "." << VERSION_MINOR << "." << VERSION_PATCH << std::endl;
    lib1();
    lib2();
    return EXIT_SUCCESS;
}
