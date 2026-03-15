cmake_minimum_required(VERSION 4.0)
project(cpp)

set(CMAKE_CXX_STANDARD 26)

add_executable(cpp
        main.cpp
        language_support_library/program_utilities/signals.cpp
        language_support_library/program_utilities/memory.cpp
        memory_management_library/pointers.cpp
        metaprogramming_library/automatic_unit_conversion.cpp
        metaprogramming_library/recursion.cpp
        ranges_library/introduction.cpp
        ranges_library/various_demos.cpp
        numerics_library/pseudo_random_number_generation.cpp
        snippets/performance.cpp
        metaprogramming_library/various_demos.cpp
)
