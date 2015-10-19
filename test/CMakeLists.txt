# Strange, need to use static linking here or tests fail.
#if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" )
#  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")
#endif()

#set(CMAKE_CXX_COMPILER_FLAGS "${CMAKE_CXX_COMPILER_FLAGS} -g")

add_subdirectory(action)
add_subdirectory(algorithm)
add_subdirectory(numeric)
add_subdirectory(utility)
add_subdirectory(view)

add_executable(container_conversion container_conversion.cpp)
add_test(test.container_conversion container_conversion)

add_executable(constexpr_core constexpr_core.cpp)
add_test(test.constexpr_core constexpr_core)

add_executable(view_facade view_facade.cpp)
add_test(test.view_facade view_facade)

add_executable(view_adaptor view_adaptor.cpp)
add_test(test.view_adaptor view_adaptor)

add_executable(range range.cpp)
add_test(test.range range)

add_executable(multiple multiple1.cpp multiple2.cpp)
add_test(test.multiple multiple)

add_executable(distance distance.cpp)
add_test(test.distance distance)

add_executable(to_container to_container.cpp)
add_test(test.to_container, to_container)

add_executable(getlines getlines.cpp)
add_test(test.getlines, getlines)

add_executable(istream_range istream_range.cpp)
add_test(test.istream_range, istream_range)
