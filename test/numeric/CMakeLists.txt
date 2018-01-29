add_executable(num.accumulate accumulate.cpp)
target_link_libraries(num.accumulate range-v3)
add_test(test.num.accumulate num.accumulate)

add_executable(num.adjacent_difference adjacent_difference.cpp)
target_link_libraries(num.adjacent_difference range-v3)
add_test(test.num.adjacent_difference num.adjacent_difference)

add_executable(num.inner_product inner_product.cpp)
target_link_libraries(num.inner_product range-v3)
add_test(test.num.inner_product num.inner_product)

add_executable(num.iota iota.cpp)
target_link_libraries(num.iota range-v3)
add_test(test.num.iota num.iota)

add_executable(num.partial_sum partial_sum.cpp)
target_link_libraries(num.partial_sum range-v3)
add_test(test.num.partial_sum num.partial_sum)
