cmake_minimum_required(VERSION 3.5)
project (koi)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(
	koistd SHARED
	print.c
	sdl.c
)

target_link_libraries(koistd SDL2)

install(TARGETS koistd DESTINATION lib)
