
add_subdirectory(algorithm)
add_subdirectory(utility)
add_subdirectory(view)

add_executable(range_facade range_facade.cpp)
add_test(test.range_facade range_facade)

add_executable(range_adaptor range_adaptor.cpp)
add_test(test.range_adaptor range_adaptor)
