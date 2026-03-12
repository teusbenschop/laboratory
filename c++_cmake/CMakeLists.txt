cmake_minimum_required(VERSION 4.0)
project(cpp)

set(CMAKE_CXX_STANDARD 26)

add_executable(cpp
        main.cpp
        language/exceptions.cpp
        language/operators.cpp
        language/declarations.cpp
        language/classes/design_principles.cpp
        language/classes/design_idioms.cpp
        language_support_library/program_utilities/signals.cpp
        language_support_library/program_utilities/memory.cpp
        memory_management_library/pointers.cpp
        metaprogramming_library/automatic_unit_conversion.cpp
        metaprogramming_library/recursion.cpp
        ranges_library/introduction.cpp
        ranges_library/various_demos.cpp
        numerics_library/pseudo_random_number_generation.cpp
        snippets/jthread_timer_condition_variable.cpp
        language/templates.cpp
        language/functions.cpp
        containers_library/containers.cpp
        snippets/scoped_timer.cpp
        snippets/performance.cpp
        general_utilities_library/demos.cpp
        metaprogramming_library/various_demos.cpp
)
