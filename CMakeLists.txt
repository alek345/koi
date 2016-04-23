cmake_minimum_required(VERSION 3.5)
project (koi)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(
	koi 
	src/main.cpp
	src/lexer.cpp
	src/parser.cpp
	src/bytecode.cpp
	src/context.cpp
)

include_directories(include)
