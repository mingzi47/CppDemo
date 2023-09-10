cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 11)
set(CMAKE_BUILD_TYPE DEBUG)
set(CMAKE_EXPORT_COMPILE_COMMANDS YES)

project(red-black-tree LANGUAGES CXX)


add_executable(test test.cpp)

target_include_directories(test PUBLIC include)
