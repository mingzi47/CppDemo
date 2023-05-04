cmake_minimum_required(VERSION 3.20.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)
set(CMAKE_EXPORT_COMPILE_COMMANDS YES)
set(CMAKE_BUILD_TYPE "Debug")

project(CppThreadPool LANGUAGES CXX)

include_directories(./src)

add_executable(test main.cpp)

