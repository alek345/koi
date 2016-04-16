@echo off
g++ --std=c++11 -g main.cpp lexer.cpp parser.cpp bytecode.cpp context.cpp -o koi -Wno-write-strings