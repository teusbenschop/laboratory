cmake_minimum_required(VERSION 3.31)
project(cpp)

set(CMAKE_CXX_STANDARD 26)

add_executable(cpp
        main.cpp
        scoped_timer.cpp
        binary_search.cpp
        lower_upper_bound.cpp
        any_of_all_of_none_of.cpp
)
