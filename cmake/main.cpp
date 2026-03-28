#include <cstdlib>
#include <iostream>
#include <ostream>

#include "config.h"
#include "library1.h"
#include "library2.h"

int main() {
    std::cout << "Version " << VERSION_MAJOR << "." << VERSION_MINOR << "." << VERSION_PATCH << std::endl;
    library1();
    library2();
    return EXIT_SUCCESS;
}
