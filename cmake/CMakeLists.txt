# https://cmake.org/cmake/help/v3.31/guide/tutorial/index.html


# Set the minimum CMake version required.
cmake_minimum_required(VERSION 3.31)


# Start a project by setting the project name and version.
project(laboratory
        VERSION 1.2.3
)


# Print info.
message("-- Processing ${CMAKE_CURRENT_SOURCE_DIR}")

set(laboratory_VERSION "${laboratory_VERSION_MAJOR}.${laboratory_VERSION_MINOR}.${laboratory_VERSION_PATCH}")
message("-- Software version ${laboratory_VERSION}")

# Print the install prefix.
message ("-- Install prefix ${CMAKE_INSTALL_PREFIX}")
# Set it during install: $ cmake --install . --prefix "/path"


# Set compiler warnings for this directory and everything below.
add_compile_options(-Wall -Wextra -Wpedantic)


# Make a variable default onal ON or OFF.
option(USE_LIBRARY1 "Use library 1" ON)
# Default is ON. Set it off: $ cmake .. -DUSE_LIBRARY1=OFF


# Write all variables to config.h.
configure_file(config.h.in ${PROJECT_SOURCE_DIR}/config.h)


# The C++ standard to use, and it is mandatory.
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)


# (Conditionally) build a static library from the given sources.
if(USE_LIBRARY1)
add_library(library1 STATIC
        library1/code.cpp
)
endif()
add_library(library2 STATIC
        library2/code.cpp
)

# Where to look for the built libraries.
link_directories(build)


# Create an executable based on the specified sources.
add_executable(laboratory
        main.cpp
)


# The include directories for the cmake target.
target_include_directories(laboratory PUBLIC
        .
)


# The target is conditionally linked against the given libraries.
if(USE_LIBRARY1)
target_link_libraries(laboratory
        library1
)
endif()
target_link_libraries(laboratory
        library2
)


# Installation.
install(TARGETS laboratory DESTINATION bin)

