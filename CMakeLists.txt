# Copyright Eric Niebler 2014
# Copyright Gonzalo Brito Gadeschi 2014, 2017
# Copyright Louis Dionne 2015
# Copyright Casey Carter 2016
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

cmake_minimum_required(VERSION 3.6)
get_directory_property(is_subproject PARENT_DIRECTORY)

if(NOT is_subproject)
    set(is_standalone YES)
else()
    set(is_standalone NO)
endif()

project(Range-v3 CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # Export compilation data-base
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

add_library(meta INTERFACE)
target_include_directories(meta INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>)
target_include_directories(meta SYSTEM INTERFACE $<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include>)

add_library(range-v3 INTERFACE)
target_include_directories(range-v3 INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>)
target_include_directories(range-v3 SYSTEM INTERFACE $<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include>)
target_link_libraries(range-v3 INTERFACE meta)

function(rv3_add_test TESTNAME EXENAME FIRSTSOURCE)
  add_executable(${EXENAME} ${FIRSTSOURCE} ${ARGN})
  target_link_libraries(${EXENAME} range-v3)
  add_test(${TESTNAME} ${EXENAME})
endfunction(rv3_add_test)

include(TestHeaders)
include(ranges_options)
include(ranges_env)
include(ranges_flags)

if(RANGE_V3_DOCS)
  add_subdirectory(doc)
endif()

if(RANGE_V3_TESTS)
  include(CTest)
  add_subdirectory(test)
endif()

if(RANGE_V3_EXAMPLES)
  add_subdirectory(example)
endif()

if(RANGE_V3_PERF)
  add_subdirectory(perf)
endif()

# Test all headers
file(GLOB_RECURSE RANGE_V3_PUBLIC_HEADERS
                  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/include"
                  "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp")

# Add header files as sources to fix MSVS 2017 not finding source during debugging
file(GLOB_RECURSE RANGE_V3_PUBLIC_HEADERS_ABSOLUTE
                  "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp")

include(TestHeaders)
if(RANGE_V3_HEADER_CHECKS)
  add_custom_target(headers ALL SOURCES ${RANGE_V3_PUBLIC_HEADERS_ABSOLUTE})
else()
  add_custom_target(headers SOURCES ${RANGE_V3_PUBLIC_HEADERS_ABSOLUTE})
endif()
generate_standalone_header_tests(EXCLUDE_FROM_ALL MASTER_TARGET headers HEADERS ${RANGE_V3_PUBLIC_HEADERS})
set_target_properties(headers
    PROPERTIES FOLDER "test/header"
)

# Grab the range-v3 version numbers:
include(${CMAKE_CURRENT_SOURCE_DIR}/Version.cmake)
set(RANGE_V3_VERSION ${RANGE_V3_MAJOR}.${RANGE_V3_MINOR}.${RANGE_V3_PATCHLEVEL})

# Try to build a new version.hpp
configure_file(version.hpp.in include/range/v3/version.hpp @ONLY)
file(STRINGS ${CMAKE_CURRENT_BINARY_DIR}/include/range/v3/version.hpp RANGE_V3_OLD_VERSION_HPP)
file(STRINGS ${CMAKE_CURRENT_SOURCE_DIR}/include/range/v3/version.hpp RANGE_V3_NEW_VERSION_HPP)

# If the new version.hpp is materially different from the one in the source
# directory, update it, commit, and tag.
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
  execute_process(
    COMMAND ${GIT_EXECUTABLE} -C "${CMAKE_CURRENT_SOURCE_DIR}" add -u
  )
  execute_process(
    COMMAND ${GIT_EXECUTABLE} -C "${CMAKE_CURRENT_SOURCE_DIR}" commit -m "${RANGE_V3_VERSION}"
  )
  execute_process(
    COMMAND ${GIT_EXECUTABLE} -C "${CMAKE_CURRENT_SOURCE_DIR}" tag -f -a "${RANGE_V3_VERSION}" -m "${RANGE_V3_VERSION}"
  )
  find_program(CONAN_EXECUTABLE NAMES conan conan.exe)
  if (NOT "x${CONAN_EXECUTABLE}" STREQUAL "xCONAN_EXECUTABLE-NOTFOUND")
    message(STATUS "Exporting conanfile for new version")
    execute_process(
      COMMAND ${CONAN_EXECUTABLE} create . range-v3/${RANGE_V3_VERSION}@ericniebler/stable
      WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    )
  endif()
  message(STATUS "Version updated to ${RANGE_V3_VERSION}. Don't forget to:")
  message(STATUS "  git push origin <feature-branch>")
  message(STATUS "and (after that is merged to master) then:")
  message(STATUS "  conan create ${CMAKE_CURRENT_SOURCE_DIR} range-v3/${RANGE_V3_VERSION}@ericniebler/stable")
  message(STATUS "  conan upload --all range-v3/${RANGE_V3_VERSION}@ericniebler/stable")
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

export(EXPORT range-v3-targets FILE range-v3-config.cmake)
