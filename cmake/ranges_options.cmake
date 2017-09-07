# Copyright Gonzalo Brito Gadeschi 2015
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)
#
# CMake options

include(CMakeDependentOption)

set(RANGES_CXX_STD 11 CACHE STRING "C++ standard version.")
option(RANGE_V3_NO_HEADER_CHECK "Build all headers." OFF)
option(RANGE_BUILD_CALENDAR_EXAMPLE "Builds the calendar example." ON)
option(RANGES_ASAN "Run the tests using AddressSanitizer." OFF)
option(RANGES_MSAN "Run the tests using MemorySanitizer." OFF)
option(RANGES_ASSERTIONS "Enable assertions." ON)
option(RANGES_DEBUG_INFO "Include debug information in the binaries." ON)
option(RANGES_MODULES "Enables the Modules TS." OFF)
option(RANGES_NATIVE "Enables -march/-mtune=native." ON)
option(RANGES_VERBOSE_BUILD "Enables debug output from CMake." OFF)
option(RANGES_LLVM_POLLY "Enables LLVM Polly." OFF)
set(RANGES_INLINE_THRESHOLD -1 CACHE STRING "Force a specific inlining threshold.")

# Enable verbose configure when passing -Wdev to CMake
if (DEFINED CMAKE_SUPPRESS_DEVELOPER_WARNINGS AND
    NOT CMAKE_SUPPRESS_DEVELOPER_WARNINGS)
  set(RANGES_VERBOSE_BUILD ON)
endif()

if (RANGES_VERBOSE_BUILD)
  message("[range-v3]: verbose build enabled.")
endif()
