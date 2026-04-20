cmake_minimum_required(VERSION 3.31)
project(cpp)

set(CMAKE_CXX_STANDARD 26)

add_executable(cpp
        main.cpp
        scoped_timer.cpp
        searching.cpp
        bounds_limits.cpp
        counting.cpp
        latches_barriers.cpp
        constraints.cpp
        fold_expressions.cpp
        containers.cpp
        containers.h
        language.cpp
        templates.cpp
        shared_mutex.cpp
        variables.cpp
        concurrency.cpp
        functional.cpp
        bad_coding.cpp
        expected.cpp
        text.cpp
        classes.cpp
        modules.cpp
        coroutines.cpp
        exceptions.cpp
        transformations.cpp
        bits.cpp

)
