
cmake_minimum_required(VERSION 2.8)

project(range_v3 CXX)
enable_testing()

include_directories(include)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11 -Wno-unused-function")

add_subdirectory(test)
