# Strange, need to use static linking here or tests fail.
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" )
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")
endif()

set(CMAKE_CXX_COMPILER_FLAGS "${CMAKE_CXX_COMPILER_FLAGS} -g")


include(CMakeParseArguments)
function(cxx_test)
    set(options )
    set(oneValueArgs PRETTY_NAME BIN_NAME)
    set(multiValueArgs SOURCES LIBS)
    CMAKE_PARSE_ARGUMENTS(OPT "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if (NOT OPT_PRETTY_NAME OR NOT OPT_BIN_NAME OR NOT OPT_SOURCES)
        message(FATAL_ERROR "Arguments PRETTY_NAME, BIN_NAME, SOURCES are required!")
    endif()

    add_executable(${OPT_BIN_NAME} ${OPT_SOURCES})
    target_link_libraries(${OPT_BIN_NAME} -pthread ${OPT_LIBS})
    add_test(${OPT_PRETTY_NAME} ${OPT_BIN_NAME})
endfunction()


add_subdirectory(action)
add_subdirectory(algorithm)
add_subdirectory(numeric)
add_subdirectory(utility)
add_subdirectory(view)

cxx_test(
  PRETTY_NAME test.container_conversion
  BIN_NAME container_conversion
  SOURCES container_conversion.cpp
)

cxx_test(
  PRETTY_NAME test.constexpr_core
  BIN_NAME constexpr_core
  SOURCES constexpr_core.cpp
)

cxx_test(
  PRETTY_NAME test.view_facade
  BIN_NAME view_facade
  SOURCES view_facade.cpp
)

cxx_test(
  PRETTY_NAME test.view_adaptor
  BIN_NAME view_adaptor
  SOURCES view_adaptor.cpp
)

cxx_test(
  PRETTY_NAME test.range
  BIN_NAME range
  SOURCES range.cpp
)

cxx_test(
  PRETTY_NAME test.multiple
  BIN_NAME multiple
  SOURCES multiple1.cpp multiple2.cpp
)

cxx_test(
  PRETTY_NAME test.distance
  BIN_NAME distance
  SOURCES distance.cpp
)

cxx_test(
  PRETTY_NAME test.to_container
  BIN_NAME to_container
  SOURCES to_container.cpp
)

cxx_test(
  PRETTY_NAME test.getlines
  BIN_NAME getlines
  SOURCES getlines.cpp
)
