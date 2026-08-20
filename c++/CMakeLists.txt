cmake_minimum_required(VERSION 3.28.3)
set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "d0edc3af-4c50-42ea-a356-e2862fe7a444")
set(CMAKE_CXX_MODULE_STD ON)

project(cpp)

set(CMAKE_CXX_STANDARD 26)

add_executable(cpp
        main.cpp
        clocking.cpp
        searching.cpp
        bounds_limits.cpp
        counting.cpp
        latches_barriers.cpp
	concepts.cpp
        containers.cpp
        containers.h
        language.cpp
        templates.cpp
        shared_mutex.cpp
        variables.cpp
        concurrency.cpp
        functional.cpp
        good_coding.cpp
        expected.cpp
        text.cpp
        classes.cpp
        modules.cpp
        coroutines.cpp
        exceptions.cpp
        transformations.cpp
        bits.cpp
        filesystem.cpp
        linux.cpp
        unions.cpp
        mathematics.cpp
        performance.cpp
        templates_explicit.cpp
        design_patterns.cpp

)
