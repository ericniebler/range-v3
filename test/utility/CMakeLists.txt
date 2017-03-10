add_executable(utility.basic_iterator basic_iterator.cpp)
add_test(test.utility.basic_iterator utility.basic_iterator)

add_executable(utility.concepts concepts.cpp)
add_test(test.utility.concepts utility.concepts)

add_executable(utility.common_type common_type.cpp)
add_test(test.utility.common_type utility.common_type)

add_executable(utility.functional functional.cpp)
add_test(test.utility.functional utility.functional)

add_executable(utility.iterator iterator.cpp)
add_test(test.utility.iterator utility.iterator)

add_executable(utility.common_iterator common_iterator.cpp)
add_test(test.utility.common_iterator utility.common_iterator)

add_executable(utility.reverse_iterator reverse_iterator.cpp)
add_test(test.utility.reverse_iterator utility.reverse_iterator)

add_executable(utility.swap swap.cpp)
add_test(test.utility.swap utility.swap)

add_executable(utility.variant variant.cpp)
add_test(test.utility.variant utility.variant)

add_executable(utility.meta meta.cpp)
add_test(test.utility.meta utility.meta)

add_executable(utility.scope_exit scope_exit.cpp)
add_test(test.utility.scope_exit utility.scope_exit)
