# https://cmake.org/cmake/help/v3.31/guide/tutorial/index.html

# Required: The minimum version.
cmake_minimum_required(VERSION 3.31)

# Start a project: Set the project name.
project(cmake VERSION 1.2.3)

set(cmake_VERSION "${cmake_VERSION_MAJOR}.${cmake_VERSION_MINOR}.${cmake_VERSION_PATCH}")
message("-- Software version ${cmake_VERSION}")

# Write all variables to config..
configure_file(config.h.in ${PROJECT_SOURCE_DIR}/config.h)

# The C++ standard to use.
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)


# Build a static library from the given sources.
add_library(library STATIC
        library1.cpp
        library2.cpp
)

link_directories(build)

# Create an executable based on specified sources.
add_executable(cmake
        main.cpp
)

# The include directories for the cmake target.
target_include_directories(cmake PUBLIC
        .
)

# The target is linked against the given libraries.
target_link_libraries(cmake
        library
)

