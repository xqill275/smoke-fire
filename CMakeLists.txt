cmake_minimum_required(VERSION 3.31)
project(untitled)

set(CMAKE_CXX_STANDARD 20)

add_executable(untitled src/main.cpp
        src/header/tokenisation.h
        src/header/Parser.h
        src/header/CodeGen.h)
