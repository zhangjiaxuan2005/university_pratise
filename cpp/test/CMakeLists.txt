cmake_minimum_required(VERSION 3.29)
project(test)

set(CMAKE_CXX_STANDARD 20)

add_executable(test main.cpp)

add_executable(test1 cmake-build-debug/test1.cpp)