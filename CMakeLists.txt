
cmake_minimum_required(VERSION 2.8)
set(CMAKE_LEGACY_CYGWIN_WIN32 0)

# set(CMAKE_C_COMPILER /bin/gcc.exe)
# set(CMAKE_CXX_COMPILER /bin/g++.exe)
# set(CMAKE_C_COMPILER /usr/local/clang-trunk/bin/clang.exe)
# set(CMAKE_CXX_COMPILER /usr/local/clang-trunk/bin/clang++.exe)

project(range_v3 CXX)
enable_testing()

include_directories(include)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11 -Wno-unused-function")

add_subdirectory(test)
add_subdirectory(example)
add_subdirectory(libs/range/v3/scratch)
