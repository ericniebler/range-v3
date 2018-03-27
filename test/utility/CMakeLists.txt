add_executable(utility.basic_iterator basic_iterator.cpp)
target_link_libraries(utility.basic_iterator range-v3)
add_test(test.utility.basic_iterator utility.basic_iterator)

add_executable(utility.concepts concepts.cpp)
target_link_libraries(utility.concepts range-v3)
add_test(test.utility.concepts utility.concepts)

add_executable(utility.common_type common_type.cpp)
target_link_libraries(utility.common_type range-v3)
add_test(test.utility.common_type utility.common_type)

add_executable(utility.functional functional.cpp)
target_link_libraries(utility.functional range-v3)
add_test(test.utility.functional utility.functional)

add_executable(utility.iterator iterator.cpp)
target_link_libraries(utility.iterator range-v3)
add_test(test.utility.iterator utility.iterator)

add_executable(utility.common_iterator common_iterator.cpp)
target_link_libraries(utility.common_iterator range-v3)
add_test(test.utility.common_iterator utility.common_iterator)

add_executable(utility.reverse_iterator reverse_iterator.cpp)
target_link_libraries(utility.reverse_iterator range-v3)
add_test(test.utility.reverse_iterator utility.reverse_iterator)

add_executable(utility.swap swap.cpp)
target_link_libraries(utility.swap range-v3)
add_test(test.utility.swap utility.swap)

add_executable(utility.variant variant.cpp)
target_link_libraries(utility.variant range-v3)
add_test(test.utility.variant utility.variant)

add_executable(utility.meta meta.cpp)
target_link_libraries(utility.meta range-v3)
add_test(test.utility.meta utility.meta)

add_executable(utility.scope_exit scope_exit.cpp)
target_link_libraries(utility.scope_exit range-v3)
add_test(test.utility.scope_exit utility.scope_exit)
