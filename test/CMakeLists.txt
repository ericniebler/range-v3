# Strange, need to use static linking here or tests fail.
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" )
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")
endif()

set(CMAKE_CXX_COMPILER_FLAGS "${CMAKE_CXX_COMPILER_FLAGS} -g")

add_subdirectory(algorithm)
add_subdirectory(extension)
add_subdirectory(numeric)
add_subdirectory(utility)
add_subdirectory(view)

add_executable(range_facade range_facade.cpp)
add_test(test.range_facade range_facade)

add_executable(range_adaptor range_adaptor.cpp)
add_test(test.range_adaptor range_adaptor)
