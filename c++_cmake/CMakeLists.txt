cmake_minimum_required(VERSION 3.6)
project(cpp)

set(CMAKE_CXX_STANDARD 26)

add_executable(cpp
        main.cpp
        language_support_library/program_utilities/signals.cpp
        ranges_library/introduction.cpp
        ranges_library/introduction.h
        metaprogramming_library/automatic_unit_conversion.cpp
        metaprogramming_library/automatic_unit_conversion.h
)
