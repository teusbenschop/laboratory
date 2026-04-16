cmake_minimum_required(VERSION 3.31)
project(cpp)

set(CMAKE_CXX_STANDARD 26)

add_executable(cpp
        main.cpp
        scoped_timer.cpp
        binary_search.cpp
        bounds_limits.cpp
        any_of_all_of_none_of.cpp
        counting.cpp
        latches_barriers.cpp
        constraints.cpp
        fold_expressions.cpp
        containers.cpp
        containers.h
        alignment.cpp
        templates.cpp
        shared_mutex.cpp
        forwarding.cpp

)
