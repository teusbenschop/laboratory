cmake_minimum_required(VERSION 4.0)
project(cpp)

set(CMAKE_CXX_STANDARD 26)

add_executable(cpp
        main.cpp
        language/exceptions.cpp
        language/classes/design_principles.cpp
        language/classes/design_idioms.cpp
        language_support_library/program_utilities/signals.cpp
        memory_management_library/pointers.cpp
        metaprogramming_library/automatic_unit_conversion.cpp
        metaprogramming_library/recursion.cpp
        ranges_library/introduction.cpp
        ranges_library/various_demos.cpp
        concurrency_support_library/mutual_exclusion/shared_and_unique_locks.cpp
        concurrency_support_library/mutual_exclusion/timed_mutex.cpp
        concurrency_support_library/mutual_exclusion/latches_and_barriers.cpp
)
