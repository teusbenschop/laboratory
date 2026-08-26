cmake_minimum_required(VERSION 3.31)
project(lab VERSION 1.2.3)
add_compile_options(-Wall -Wextra -Wpedantic)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED true)

add_executable(demo
	application.cpp
	application.h
	main.cpp
	main.h
	panels-demo.cpp
	panels-demo.h
	item-quit.cpp
	item-quit.h
	form-label.cpp
	form-label.h
	form-user-field.cpp
	form-user-field.h
	form-demo.cpp
	form-demo.h
	item-form.cpp
	item-form.h
	item-pad.cpp
	item-pad.h
	item-passive.cpp
	item-passive.h
	item-scan.cpp
	item-scan.h
	menu-main.cpp
	menu-main.h
	userdata.cpp
	userdata.h

)

target_include_directories(demo PRIVATE
	${CMAKE_SOURCE_DIR}
)

find_package(PkgConfig)
pkg_check_modules(FORM REQUIRED form)
pkg_check_modules(MENU REQUIRED menu)
pkg_check_modules(PANEL REQUIRED panel)
pkg_check_modules(NCURSES REQUIRED ncurses)
pkg_check_modules(NCURSESPP REQUIRED ncurses++)

target_link_libraries(demo
	${NCURSESPP_LIBRARIES}
	${FORM_LIBRARIES}
	${MENU_LIBRARIES}
	${PANEL_LIBRARIES}
	${NCURSES_LIBRARIES}
)

