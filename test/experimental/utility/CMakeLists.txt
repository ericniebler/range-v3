
add_executable(generator generator.cpp)
target_link_libraries(generator range-v3)
add_test(test.generator generator)
