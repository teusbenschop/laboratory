cmake_minimum_required(VERSION 3.31)
project(lab VERSION 1.2.3)
add_compile_options(-Wall -Wextra -Wpedantic)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED true)
add_executable(lab
        main.cpp
)
target_include_directories(lab PUBLIC
        .
)
find_package(PkgConfig REQUIRED)
pkg_check_modules(FORM REQUIRED form)
target_link_libraries(lab ${FORM_LIBRARIES})
target_include_directories(lab PUBLIC ${FORM_INCLUDE_DIRS})
target_compile_options(lab PUBLIC ${FORM_CFLAGS_OTHER})
pkg_check_modules(MENU REQUIRED menu)
target_link_libraries(lab ${MENU_LIBRARIES})
target_include_directories(lab PUBLIC ${MENU_INCLUDE_DIRS})
target_compile_options(lab PUBLIC ${MENU_CFLAGS_OTHER})
pkg_check_modules(NCURSES REQUIRED ncurses)
target_link_libraries(lab ${NCURSES_LIBRARIES})
target_include_directories(lab PUBLIC ${NCURSES_INCLUDE_DIRS})
target_compile_options(lab PUBLIC ${NCURSES_CFLAGS_OTHER})
pkg_check_modules(NCURSESPP REQUIRED ncurses++)
target_link_libraries(lab ${NCURSESPP_LIBRARIES})
target_include_directories(lab PUBLIC ${NCURSESPP_INCLUDE_DIRS})
target_compile_options(lab PUBLIC ${NCURSESPP_CFLAGS_OTHER})




