cmake_minimum_required(VERSION 3.6)
project(c___cmake)

set(CMAKE_CXX_STANDARD 20)

add_executable(c___cmake
        main.cpp
        language_support_library/program_utilities/signals.cpp
)
