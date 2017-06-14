cmake_minimum_required(VERSION 3.0)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

project(Range-v3 CXX)

find_package(Doxygen)
find_package(Git)

include_directories(include)

enable_testing()
include(CTest)
include(CheckCXXCompilerFlag)
include(CMakeDependentOption)

option(RANGES_MODULES "Enables Clang Modules." OFF)

find_program(MEMORYCHECK_COMMAND valgrind)
if(MEMORYCHECK_COMMAND)
  message("Valgrind: ${MEMORYCHECK_COMMAND}")
  set(MEMORYCHECK_COMMAND_OPTIONS "--trace-children=yes --leak-check=full")
else()
  message("Valgrind not found")
endif()

macro(range_v3_append_flag testname flag)
  check_cxx_compiler_flag(${flag} ${testname})
  if (${testname})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${flag}")
  endif()
endmacro()

# Select C++ standard to be used for compiling the tests,
# for example: 11, 14, 17, 1z, 1y, ...
#
if(NOT RANGES_CXX_STD)
  # Defaults to C++11 if not set:
  set(RANGES_CXX_STD 11)
endif()

if("x${CMAKE_CXX_COMPILER_ID}" MATCHES "x.*Clang")
  # Clang/C2 will blow up with various parts of the standard library
  # if compiling with -std less than c++14.
  if(("x${CMAKE_CXX_SIMULATE_ID}" STREQUAL "xMSVC") AND (RANGES_CXX_STD EQUAL 11))
    set(CMAKE_CXX_STANDARD 14)
    set(RANGES_CXX_STD 14)
  endif()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++${RANGES_CXX_STD} -ftemplate-backtrace-limit=0")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Weverything -Werror -pedantic-errors")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-c++98-compat -Wno-c++98-compat-pedantic")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-weak-vtables -Wno-padded")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-missing-prototypes -Wno-missing-variable-declarations")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-shadow -Wno-old-style-cast")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-documentation -Wno-documentation-unknown-command")
  if(CMAKE_SYSTEM_NAME MATCHES "Darwin")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-global-constructors -Wno-exit-time-destructors")
  endif()
  if (RANGES_MODULES)
    if(NOT RANGES_LIBCXX_MODULE)
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fmodules -fmodule-map-file=${PROJECT_SOURCE_DIR}/include/module.modulemap -fmodules-cache-path=${PROJECT_BINARY_DIR}/module.cache")
    else()
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fmodules -fmodule-map-file=${PROJECT_SOURCE_DIR}/include/module.modulemap -fmodules-cache-path=${PROJECT_BINARY_DIR}/module.cache -fno-implicit-module-maps -fmodule-map-file=${RANGES_LIBCXX_MODULE}")
    endif()
  endif()
  set(CMAKE_CXX_FLAGS_DEBUG "-O0 -fno-inline -g3 -fstack-protector-all")
  set(CMAKE_CXX_FLAGS_RELEASE "-Ofast -g0 -DNDEBUG")
elseif(CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++${RANGES_CXX_STD} -ftemplate-backtrace-limit=0")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Werror -pedantic-errors")
  if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "6.0")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-strict-overflow")
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "5.0")
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-missing-field-initializers")
    endif()
  elseif((CMAKE_CXX_COMPILER_VERSION VERSION_GREATER "7.0") OR (CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL "7.0"))
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-noexcept-type")
  endif()
  set(CMAKE_CXX_FLAGS_DEBUG "-O0 -fno-inline -g3 -fstack-protector-all")
  set(CMAKE_CXX_FLAGS_RELEASE "-Ofast -g0 -DNDEBUG")
# else()
#   message(FATAL_ERROR "Unknown compiler. Good luck!")
endif()

range_v3_append_flag(RANGE_V3_HAS_MARCH_NATIVE "-march=native")
range_v3_append_flag(RANGE_V3_HAS_MTUNE_NATIVE "-mtune=native")

# Test all headers
file(GLOB_RECURSE RANGE_V3_PUBLIC_HEADERS
                  RELATIVE "${CMAKE_SOURCE_DIR}/include"
                  "${CMAKE_SOURCE_DIR}/include/*.hpp")

# Add header files as sources to fix MSVS 2017 not finding source during debugging
file(GLOB_RECURSE RANGE_V3_PUBLIC_HEADERS_ABSOLUTE
                  "${CMAKE_SOURCE_DIR}/include/*.hpp")

include(TestHeaders)
if(RANGE_V3_NO_HEADER_CHECK)
  add_custom_target(headers SOURCES ${RANGE_V3_PUBLIC_HEADERS_ABSOLUTE})
else()
  add_custom_target(headers ALL SOURCES ${RANGE_V3_PUBLIC_HEADERS_ABSOLUTE})
endif()
generate_standalone_header_tests(EXCLUDE_FROM_ALL MASTER_TARGET headers HEADERS ${RANGE_V3_PUBLIC_HEADERS})

# Grab the range-v3 version numbers:
include(${CMAKE_SOURCE_DIR}/Version.cmake)
set(RANGE_V3_VERSION ${RANGE_V3_MAJOR}.${RANGE_V3_MINOR}.${RANGE_V3_PATCHLEVEL})

# Try to build a new version.hpp
configure_file(version.hpp.in include/range/v3/version.hpp @ONLY)
file(STRINGS ${CMAKE_CURRENT_BINARY_DIR}/include/range/v3/version.hpp RANGE_V3_OLD_VERSION_HPP)
file(STRINGS ${CMAKE_CURRENT_SOURCE_DIR}/include/range/v3/version.hpp RANGE_V3_NEW_VERSION_HPP)

# If the new version.hpp is materially different from the one in the source
# directory, update it, amend the most recent commit, and tag the commit.
if(NOT RANGE_V3_NEW_VERSION_HPP STREQUAL RANGE_V3_OLD_VERSION_HPP)
  # Check that Version.cmake is the only changed file:
  execute_process(
    COMMAND ${GIT_EXECUTABLE} -C "${CMAKE_CURRENT_SOURCE_DIR}" status -s -uno
    OUTPUT_VARIABLE RANGE_V3_GIT_STATUS
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )
  if (NOT "x${RANGE_V3_GIT_STATUS}" STREQUAL "x M Version.cmake")
    message(FATAL_ERROR "Cannot update version.hpp: range-v3 source directory has a dirty status")
  endif()
  file(
    COPY ${CMAKE_CURRENT_BINARY_DIR}/include/range/v3/version.hpp
    DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/include/range/v3
  )
  # Update the conanfile, too
  configure_file(conanfile.py.in ${CMAKE_CURRENT_SOURCE_DIR}/conanfile.py @ONLY)
  execute_process(
    COMMAND ${GIT_EXECUTABLE} -C "${CMAKE_CURRENT_SOURCE_DIR}" add -u
  )
  execute_process(
    COMMAND ${GIT_EXECUTABLE} -C "${CMAKE_CURRENT_SOURCE_DIR}" commit --amend --no-edit
  )
  execute_process(
    COMMAND ${GIT_EXECUTABLE} -C "${CMAKE_CURRENT_SOURCE_DIR}" tag -f -a "${RANGE_V3_VERSION}" -m "${RANGE_V3_VERSION}"
  )
  find_program(CONAN_EXECUTABLE NAMES conan conan.exe)
  if (NOT "x${CONAN_EXECUTABLE}" STREQUAL "xCONAN_EXECUTABLE-NOTFOUND")
    message("Exporting conanfile for new version")
    execute_process(
      COMMAND ${CONAN_EXECUTABLE} export ericniebler/stable
      WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    )
  endif()
  message("Version updated to @{RANGE_V3_VERSION}. Don't forget to:")
  message("  git push origin <feature-branch>")
  message("and (after that is merged to master) then:")
  message("  conan upload range-v3/${RANGE_V3_VERSION}@ericniebler/stable")
endif()

add_subdirectory(doc)
add_subdirectory(test)
add_subdirectory(example)
add_subdirectory(perf)

install(DIRECTORY include/ DESTINATION include
        FILES_MATCHING PATTERN "*.hpp")
