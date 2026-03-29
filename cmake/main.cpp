#include <cstdlib>
#include <iostream>
#include <ostream>

#include "config.h"
#include "library1/code.h"
#include "library2/code.h"

int main() {
    std::cout << "Version " << VERSION_MAJOR << "." << VERSION_MINOR << "." << VERSION_PATCH << std::endl;
#ifdef USE_LIBRARY1
    library1();
#endif
    library2();
    return EXIT_SUCCESS;
}
