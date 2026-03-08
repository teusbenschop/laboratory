#include <iostream>
#include <memory>
#include "functions.h"

namespace memory_management_library {

void pointer_types()
{
    const auto print = [](const auto& ptr) -> void {
        std::cout << "Object value: " << *ptr << std::endl;
    };

    // It is fine to pass a raw pointer to a function or in the code.
    // It means that something else is the "owner" of the object pointed at.
    {
        constexpr int integer = 10;
        const int* int_ptr = &(integer);
        print(int_ptr);
    }

    // It is not normally okay to use a shared pointer.

    // It is fine to use a unique pointer.
    {
        const auto int_ptr = std::make_unique<int>(10);
        print(int_ptr);
    }
}


}