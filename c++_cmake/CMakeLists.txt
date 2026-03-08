cmake_minimum_required(VERSION 4.0)
project(cpp)

set(CMAKE_CXX_STANDARD 26)

add_executable(cpp
        main.cpp
        language_support_library/program_utilities/signals.cpp
        ranges_library/introduction.cpp
        metaprogramming_library/automatic_unit_conversion.cpp
        language/exceptions.cpp
)
