cmake_minimum_required(VERSION 3.20.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(ejson LANGUAGES CXX)

include_directories(include)

add_executable(test test.cpp)
