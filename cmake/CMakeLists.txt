cmake_minimum_required(VERSION 3.31)

set(CMAKE_CXX_STANDARD 20)

include_directories(.)

add_library(library STATIC
        lib1.cpp
        lib2.cpp
)

link_directories(build)

link_libraries(library)

project(cmake)

add_executable(cmake
        main.cpp
)
