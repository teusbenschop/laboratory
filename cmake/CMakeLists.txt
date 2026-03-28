# https://cmake.org/cmake/help/v3.31/guide/tutorial/index.html

# Required: The minimum version.
cmake_minimum_required(VERSION 3.31)

# Start a project: Set the project name.
project(cmake VERSION 1.2.3)

# Write version number to version.h.
configure_file(version.h.in ${PROJECT_SOURCE_DIR}/version.h)

# The C++ standard to use.
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)

include_directories(.)

add_library(library STATIC
        lib1.cpp
        lib2.cpp
)

link_directories(build)

link_libraries(library)

# Create an executable based on specified sources.
add_executable(cmake
        main.cpp
)
