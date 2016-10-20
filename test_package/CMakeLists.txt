PROJECT(PackageTest)
cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_executable(example example.cpp)

target_link_libraries(example ${CONAN_LIBS})

target_compile_features(example PRIVATE cxx_range_for)