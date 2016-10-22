# Range v3 library
#
#  Copyright Luis Martinez de Bartolome Izquierdo 2016
#
#  Use, modification and distribution is subject to the
#  Boost Software License, Version 1.0. (See accompanying
#  file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt)
#
# Project home: https://github.com/ericniebler/range-v3
#

# This example shows how to define a range that is implemented
# in terms of itself. The example is generating the Fibonacci
# sequence using self-reference and zip_with.
#
# Note: don't use recursive_range_fn in performance sensitive
# code. Self-reference comes with indirection and dynamic
# allocation overhead.

PROJECT(PackageTest)
cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_executable(example example.cpp)

target_link_libraries(example ${CONAN_LIBS})

target_compile_features(example PRIVATE cxx_range_for)