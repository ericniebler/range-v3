cmake_minimum_required(VERSION 2.8)
set(CMAKE_LEGACY_CYGWIN_WIN32 0)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

project(Range-v3 CXX)

find_package(Doxygen)
find_package(Git)

include_directories(include)

enable_testing()
include(CTest)
include(CheckCXXCompilerFlag)
find_program(MEMORYCHECK_COMMAND valgrind)
if(MEMORYCHECK_COMMAND-NOTFOUND)
  message("[W] Valgrind not found")
else()
  message("Valgrind: ${MEMORYCHECK_COMMAND}")
  set(MEMORYCHECK_COMMAND_OPTIONS "--trace-children=yes --leak-check=full")
endif()

macro(range_v3_append_flag testname flag)
  check_cxx_compiler_flag(${flag} ${testname})
  if (${testname})
    add_compile_options(${flag})
  endif()
endmacro()

# Select C++ standard to be used for compiling the tests,
# for example: 11, 14, 17, 1z, 1y, ...
#
if(NOT RANGES_CXX_STD)
  # Defaults to C++11 if not set:
  set(RANGES_CXX_STD 11)
endif()

if("x${CMAKE_CXX_COMPILER_ID}" STREQUAL "xClang")
  # Clang/C2 will blow up with various parts of the standard library
  # if compiling with -std less than c++14.
  if(("x${CMAKE_CXX_SIMULATE_ID}" STREQUAL "xMSVC") AND
     ("${RANGES_CXX_STD}" STREQUAL "11"))
    set(CMAKE_CXX_STANDARD 14)
    set(RANGES_CXX_STD 14)
  endif()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++${RANGES_CXX_STD} -ftemplate-backtrace-limit=0")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Weverything -Werror -pedantic-errors")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-c++98-compat -Wno-c++98-compat-pedantic")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-weak-vtables -Wno-padded -Wno-undef")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-missing-prototypes -Wno-missing-variable-declarations")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-shadow -Wno-old-style-cast")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-documentation -Wno-documentation-unknown-command")
  if("${CMAKE_SYSTEM_NAME}" MATCHES "Darwin")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-global-constructors -Wno-exit-time-destructors")
  endif()
  set(CMAKE_CXX_FLAGS_DEBUG "-O0 -fno-inline -g3 -fstack-protector-all")
  set(CMAKE_CXX_FLAGS_RELEASE "-Ofast -g0 -DNDEBUG")
elseif(CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++${RANGES_CXX_STD} -ftemplate-backtrace-limit=0")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Werror -pedantic-errors")
  if("${CMAKE_CXX_COMPILER_VERSION}" VERSION_LESS "5.0")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-missing-field-initializers")
  endif()
  set(CMAKE_CXX_FLAGS_DEBUG "-O0 -fno-inline -g3 -fstack-protector-all")
  set(CMAKE_CXX_FLAGS_RELEASE "-Ofast -g0 -DNDEBUG")
# else()
#   message(FATAL_ERROR "Unknown compiler. Good luck!")
endif()

range_v3_append_flag(RANGE_V3_HAS_MARCH_NATIVE "-march=native")
range_v3_append_flag(RANGE_V3_HAS_MTUNE_NATIVE "-mtune=native")

#   range_v3_list_remove_glob(<list> <GLOB|GLOB_RECURSE> [globbing expressions]...)
#
# Generates a list of files matching the given glob expressions, and remove
# the matched elements from the given <list>.
#
# Adapted from Boost.Hana: https://github.com/ldionne/hana/
#
macro(range_v3_list_remove_glob list glob)
    file(${glob} _bhlrg10321023_avoid_macro_clash_matches ${ARGN})
    list(REMOVE_ITEM ${list} ${_bhlrg10321023_avoid_macro_clash_matches})
endmacro()

# Test all headers
if(NOT RANGE_V3_NO_HEADER_CHECK)
  file(GLOB_RECURSE RANGE_V3_PUBLIC_HEADERS
                    RELATIVE "${CMAKE_SOURCE_DIR}/include"
                    "${CMAKE_SOURCE_DIR}/include/*.hpp")
  include(TestHeaders)
  generate_standalone_header_tests(HEADERS ${RANGE_V3_PUBLIC_HEADERS})
endif()

add_subdirectory(doc)
add_subdirectory(test)
add_subdirectory(example)
add_subdirectory(perf)

install(DIRECTORY include/ DESTINATION include
        FILES_MATCHING PATTERN "*.hpp")
