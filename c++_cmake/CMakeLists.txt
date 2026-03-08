cmake_minimum_required(VERSION 3.6)
project(cpp)

set(CMAKE_CXX_STANDARD 20)

add_executable(cpp
        main.cpp
        language_support_library/program_utilities/signals.cpp
)
