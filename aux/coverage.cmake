################################################################################
# Coverage target: make cov
################################################################################
include(CMakeDependentOption)
option(RANGE_V3_ANALYZE_COVERAGE "Analyzes test coverage" OFF)

if(RANGE_V3_ANALYZE_COVERAGE)
  message("Coverage analysis enabled.")
  if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
    message("[Error] Coverage analysis requires debug information.")
    exit()
  endif()
  if("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage -Xclang -coverage-cfg-checksum -Xclang -coverage-no-function-names-in-data -Xclang -coverage-version='407*'")
  elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    message("[Error] Coverage analysis with GCC is untested.")
    exit()
    # This should "just work":
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
  else()
    message("[Error] Coverage analysis with the ${CMAKE_CXX_COMPILER_ID} compiler is unimplemented yet")
    exit()
  endif()
  # LCov (coverage): make cov
  add_custom_command(OUTPUT coverage COMMAND
    ${PROJECT_SOURCE_DIR}/aux/coverage.sh "${PROJECT_SOURCE_DIR}/include/")
  add_custom_target(cov DEPENDS coverage WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
endif()
