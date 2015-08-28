cmake_minimum_required(VERSION 2.8)
set(CMAKE_LEGACY_CYGWIN_WIN32 0)

project(Range-v3 CXX)

find_package(Doxygen)
find_package(Git)

# Select C++ standard to be used for compiling the tests,
# for example: 11, 14, 17, 1z, 1y, ...
#
if(NOT RANGES_CXX_STD)
  # Defaults to C++11 if not set:
  set(RANGES_CXX_STD 11) 
endif()

enable_testing()
include(CTest)
find_program(MEMORYCHECK_COMMAND valgrind)
if(MEMORYCHECK_COMMAND-NOTFOUND)
  message("[W] Valgrind not found")
else()
  message("Valgrind: ${MEMORYCHECK_COMMAND}")
  set(MEMORYCHECK_COMMAND_OPTIONS "--trace-children=yes --leak-check=full")
endif()

include_directories(include)

add_definitions(-DRANGES_SUPPRESS_IOTA_WARNING)
if("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
  set(CXX_FLAGS_LIST
    -std=c++${RANGES_CXX_STD} -ftemplate-backtrace-limit=0
    -Weverything -Werror -pedantic-errors
    -Wno-error=deprecated-declarations -Wno-error=deprecated
    -Wno-c++98-compat -Wno-c++98-compat-pedantic
    -Wno-return-stack-address
    -Wno-unused-variable -Wno-unused-parameter
    -Wno-weak-vtables -Wno-padded
    -Wno-global-constructors -Wno-exit-time-destructors
    -Wno-shorten-64-to-32 -Wno-sign-conversion
    -Wno-missing-prototypes -Wno-missing-variable-declarations
    -Wno-shadow -Wno-old-style-cast
    -Wno-documentation -Wno-documentation-unknown-command
  )
  string(REPLACE ";" " " CXX_FLAGS "${CXX_FLAGS_LIST}")

  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX_FLAGS}")
  set(CMAKE_CXX_FLAGS_DEBUG "-O0 -fno-inline -g3 -fstack-protector-all")
  set(CMAKE_CXX_FLAGS_RELEASE "-Ofast -g0 -march=native -mtune=native -DNDEBUG")
  set(CMAKE_CXX_FLAGS_COVERAGE "--coverage -fkeep-inline-functions")
elseif(CMAKE_COMPILER_IS_GNUCXX)
  set(CXX_FLAGS_LIST
    -std=c++${RANGES_CXX_STD} -ftemplate-backtrace-limit=0
    -Wall -Wextra -pedantic -Werror -pedantic-errors
    -Wno-error=deprecated-declarations
    -Wno-missing-field-initializers
    -Wno-unused-parameter
  )
  string(REPLACE ";" " " CXX_FLAGS "${CXX_FLAGS_LIST}")

  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX_FLAGS}")
  set(CMAKE_CXX_FLAGS_DEBUG "-O0 -fno-inline -g3 -fstack-protector-all")
  set(CMAKE_CXX_FLAGS_RELEASE "-Ofast -g0 -march=native -mtune=native -DNDEBUG")
  set(CMAKE_CXX_FLAGS_COVERAGE "--coverage -fkeep-inline-functions")
# else()
#   message(FATAL_ERROR "Unknown compiler.")
endif()


add_subdirectory(doc)

if(BIICODE)
    ADD_BIICODE_TARGETS()

    string(REPLACE " " ";" TARGET_CXX_FLAGS         ${CMAKE_CXX_FLAGS})
    string(REPLACE " " ";" TARGET_CXX_FLAGS_DEBUG   ${CMAKE_CXX_FLAGS_DEBUG})
    string(REPLACE " " ";" TARGET_CXX_FLAGS_RELEASE ${CMAKE_CXX_FLAGS_RELEASE})     

    target_compile_options(${BII_BLOCK_TARGET} INTERFACE ${TARGET_CXX_FLAGS}
                           $<$<CONFIG:Debug>:${TARGET_CXX_FLAGS_DEBUG}>
                           $<$<CONFIG:Release>:${TARGET_CXX_FLAGS_RELEASE}>)

    return()
endif()

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
include(CheckIncludeFileCXX)
file(GLOB_RECURSE RANGE_V3_PUBLIC_HEADERS "${CMAKE_SOURCE_DIR}/include/*.hpp")
range_v3_list_remove_glob(RANGE_V3_PUBLIC_HEADERS GLOB_RECURSE
  "${CMAKE_SOURCE_DIR}/include/range/v3/detail/re_enable_warnings.hpp"
  )

foreach(_header IN LISTS RANGE_V3_PUBLIC_HEADERS)
  file(RELATIVE_PATH _relative "${CMAKE_SOURCE_DIR}/include" "${_header}")
  check_include_file_cxx("${_relative}" ${_relative}_FOUND "-I${CMAKE_SOURCE_DIR}/include")
  if(NOT ${_relative}_FOUND)
    message(FATAL_ERROR "Compilation of header ${_relative} failed")
  endif()
endforeach()

add_subdirectory(test)
add_subdirectory(example)
add_subdirectory(perf)
add_subdirectory(libs/range/v3/scratch)
