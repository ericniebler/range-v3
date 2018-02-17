# Copyright Eric Niebler 2014
# Copyright Gonzalo Brito Gadeschi 2014, 2017
# Copyright Louis Dionne 2015
# Copyright Casey Carter 2016
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

cmake_minimum_required(VERSION 3.6)

project(Range-v3 CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # Export compilation data-base

include(TestHeaders)
include(ranges_options)
include(ranges_env)
include(ranges_flags)
include(CTest)

enable_testing()

add_library(meta INTERFACE)
target_include_directories(meta INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>)
target_include_directories(meta SYSTEM INTERFACE $<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include>)

add_library(range-v3 INTERFACE)
target_include_directories(range-v3 INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>)
target_include_directories(range-v3 SYSTEM INTERFACE $<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include>)
target_link_libraries(range-v3 INTERFACE meta)

add_subdirectory(doc)

if(NOT RANGE_V3_NO_TESTING)
  add_subdirectory(test)
endif()

if(NOT RANGE_V3_NO_EXAMPLE)
  add_subdirectory(example)
endif()

if(NOT RANGE_V3_NO_PERF)
  add_subdirectory(perf)
endif()

# Test for <thread>
try_compile(RANGE_V3_TRY_THREAD ${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/thread_test_code.cpp)
if(NOT RANGE_V3_TRY_THREAD)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DRANGES_CXX_THREAD=0")
endif()

# Test for coroutine TS support
include(CheckCXXSourceCompiles)

file(READ "${CMAKE_CURRENT_SOURCE_DIR}/cmake/coro_test_code.cpp" RANGE_V3_CORO_TEST_CODE)
set(CMAKE_REQUIRED_FLAGS "-fcoroutines-ts")
check_cxx_source_compiles("${RANGE_V3_CORO_TEST_CODE}" RANGE_V3_HAS_FCOROUTINES_TS)

if (RANGE_V3_HAS_FCOROUTINES_TS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcoroutines-ts")
endif()

# Test all headers
file(GLOB_RECURSE RANGE_V3_PUBLIC_HEADERS
                  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/include"
                  "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp")

# Add header files as sources to fix MSVS 2017 not finding source during debugging
file(GLOB_RECURSE RANGE_V3_PUBLIC_HEADERS_ABSOLUTE
                  "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp")

include(TestHeaders)
if(RANGE_V3_NO_HEADER_CHECK)
  add_custom_target(headers SOURCES ${RANGE_V3_PUBLIC_HEADERS_ABSOLUTE})
else()
  add_custom_target(headers ALL SOURCES ${RANGE_V3_PUBLIC_HEADERS_ABSOLUTE})
endif()
generate_standalone_header_tests(EXCLUDE_FROM_ALL MASTER_TARGET headers HEADERS ${RANGE_V3_PUBLIC_HEADERS})

# Grab the range-v3 version numbers:
include(${CMAKE_CURRENT_SOURCE_DIR}/Version.cmake)
set(RANGE_V3_VERSION ${RANGE_V3_MAJOR}.${RANGE_V3_MINOR}.${RANGE_V3_PATCHLEVEL})

# Try to build a new version.hpp
configure_file(version.hpp.in include/range/v3/version.hpp @ONLY)
file(STRINGS ${CMAKE_CURRENT_BINARY_DIR}/include/range/v3/version.hpp RANGE_V3_OLD_VERSION_HPP)
file(STRINGS ${CMAKE_CURRENT_SOURCE_DIR}/include/range/v3/version.hpp RANGE_V3_NEW_VERSION_HPP)

# If the new version.hpp is materially different from the one in the source
# directory, update it, amend the most recent commit, and tag the commit.
if(NOT RANGE_V3_NEW_VERSION_HPP STREQUAL RANGE_V3_OLD_VERSION_HPP)
  # Check that README.md and Version.cmake are the only changed file:
  execute_process(
    COMMAND ${GIT_EXECUTABLE} -C "${CMAKE_CURRENT_SOURCE_DIR}" status --porcelain -uno
    OUTPUT_VARIABLE RANGE_V3_GIT_STATUS
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )
  string(REPLACE "\n" ";"  RANGE_V3_GIT_STATUS ${RANGE_V3_GIT_STATUS})
  if (NOT "x${RANGE_V3_GIT_STATUS}" STREQUAL "x M README.md; M Version.cmake")
    message(FATAL_ERROR "Cannot update version.hpp: range-v3 source directory has a dirty status")
  endif()
  file(
    COPY ${CMAKE_CURRENT_BINARY_DIR}/include/range/v3/version.hpp
    DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/include/range/v3
  )
  # Update the conanfiles, too
  configure_file(conanfile.py.in ${CMAKE_CURRENT_SOURCE_DIR}/conanfile.py @ONLY)
  configure_file(test_package/conanfile.py.in ${CMAKE_CURRENT_SOURCE_DIR}/test_package/conanfile.py @ONLY)
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
      COMMAND ${CONAN_EXECUTABLE} export . ericniebler/stable
      WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    )
  endif()
  message("Version updated to ${RANGE_V3_VERSION}. Don't forget to:")
  message("  git push origin <feature-branch>")
  message("and (after that is merged to master) then:")
  message("  conan export ericniebler")
  message("  conan test_package")
  message("  conan upload --all range-v3/${RANGE_V3_VERSION}@ericniebler/stable")
endif()

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/range-v3-config-version.cmake
  VERSION ${RANGE_V3_VERSION}
  COMPATIBILITY ExactVersion
)

install(TARGETS meta range-v3 EXPORT range-v3-targets DESTINATION lib)
install(EXPORT range-v3-targets FILE range-v3-config.cmake DESTINATION lib/cmake/range-v3)
install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/range-v3-config-version.cmake
  DESTINATION lib/cmake/range-v3)
install(DIRECTORY include/ DESTINATION include FILES_MATCHING PATTERN "*.hpp")
