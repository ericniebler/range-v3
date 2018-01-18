add_executable(view.adjacent_remove_if adjacent_remove_if.cpp)
target_link_libraries(view.adjacent_remove_if range-v3)
add_test(test.view.adjacent_remove_if, view.adjacent_remove_if)

add_executable(view.all all.cpp)
target_link_libraries(view.all range-v3)
add_test(test.view.all, view.all)

add_executable(view.any_view any_view.cpp)
target_link_libraries(view.any_view range-v3)
add_test(test.view.any_view, view.any_view)

add_executable(view.bounded bounded.cpp)
target_link_libraries(view.bounded range-v3)
add_test(test.view.bounded, view.bounded)

add_executable(view.cartesian_product cartesian_product.cpp)
target_link_libraries(view.cartesian_product range-v3)
add_test(test.view.cartesian_product, view.cartesian_product)

add_executable(view.chunk chunk.cpp)
target_link_libraries(view.chunk range-v3)
add_test(test.view.chunk, view.chunk)

add_executable(view.concat concat.cpp)
target_link_libraries(view.concat range-v3)
add_test(test.view.concat, view.concat)

add_executable(view.const const.cpp)
target_link_libraries(view.const range-v3)
add_test(test.view.const, view.const)

add_executable(view.counted counted.cpp)
target_link_libraries(view.counted range-v3)
add_test(test.view.counted, view.counted)

add_executable(view.cycle cycle.cpp)
target_link_libraries(view.cycle range-v3)
add_test(test.view.cycle, view.cycle)

add_executable(view.delimit delimit.cpp)
target_link_libraries(view.delimit range-v3)
add_test(test.view.delimit, view.delimit)

add_executable(view.drop drop.cpp)
target_link_libraries(view.drop range-v3)
add_test(test.view.drop, view.drop)

add_executable(view.drop_exactly drop_exactly.cpp)
target_link_libraries(view.drop_exactly range-v3)
add_test(test.view.drop_exactly, view.drop_exactly)

add_executable(view.drop_while drop_while.cpp)
target_link_libraries(view.drop_while range-v3)
add_test(test.view.drop_while, view.drop_while)

add_executable(view.generate generate.cpp)
target_link_libraries(view.generate range-v3)
add_test(test.view.generate, view.generate)

add_executable(view.generate_n generate_n.cpp)
target_link_libraries(view.generate_n range-v3)
add_test(test.view.generate_n, view.generate_n)

add_executable(view.group_by group_by.cpp)
target_link_libraries(view.group_by range-v3)
add_test(test.view.group_by, view.group_by)

add_executable(view.indirect indirect.cpp)
target_link_libraries(view.indirect range-v3)
add_test(test.view.indirect, view.indirect)

add_executable(view.intersperse intersperse.cpp)
target_link_libraries(view.intersperse range-v3)
add_test(test.view.intersperse, view.intersperse)

add_executable(view.iota iota.cpp)
target_link_libraries(view.iota range-v3)
add_test(test.view.iota, view.iota)

add_executable(view.join join.cpp)
target_link_libraries(view.join range-v3)
add_test(test.view.join, view.join)

add_executable(view.linear_distribute linear_distribute.cpp)
target_link_libraries(view.linear_distribute range-v3)
add_test(test.view.linear_distribute, view.linear_distribute)

add_executable(view.map keys_value.cpp)
target_link_libraries(view.map range-v3)
add_test(test.view.map, view.map)

add_executable(view.move move.cpp)
target_link_libraries(view.move range-v3)
add_test(test.view.move, view.move)

add_executable(view.partial_sum partial_sum.cpp)
target_link_libraries(view.partial_sum range-v3)
add_test(test.view.partial_sum, view.partial_sum)

add_executable(view.repeat repeat.cpp)
target_link_libraries(view.repeat range-v3)
add_test(test.view.repeat, view.repeat)

add_executable(view.remove_if remove_if.cpp)
target_link_libraries(view.remove_if range-v3)
add_test(test.view.remove_if, view.remove_if)

add_executable(view.replace replace.cpp)
target_link_libraries(view.replace range-v3)
add_test(test.view.replace, view.replace)

add_executable(view.replace_if replace_if.cpp)
target_link_libraries(view.replace_if range-v3)
add_test(test.view.replace_if, view.replace_if)

add_executable(view.reverse reverse.cpp)
target_link_libraries(view.reverse range-v3)
add_test(test.view.reverse, view.reverse)

add_executable(view.sample sample.cpp)
target_link_libraries(view.sample range-v3)
add_test(test.view.sample, view.sample)

add_executable(view.set_difference set_difference.cpp)
target_link_libraries(view.set_difference range-v3)
add_test(test.view.set_difference, view.set_difference)

add_executable(view.set_intersection set_intersection.cpp)
target_link_libraries(view.set_intersection range-v3)
add_test(test.view.set_intersection, view.set_intersection)

add_executable(view.set_symmetric_difference set_symmetric_difference.cpp)
target_link_libraries(view.set_symmetric_difference range-v3)
add_test(test.view.set_symmetric_difference, view.set_symmetric_difference)

add_executable(view.set_union set_union.cpp)
target_link_libraries(view.set_union range-v3)
add_test(test.view.set_union, view.set_union)

add_executable(view.slice slice.cpp)
target_link_libraries(view.slice range-v3)
add_test(test.view.slice, view.slice)

add_executable(view.sliding sliding.cpp)
target_link_libraries(view.sliding range-v3)
add_test(test.view.sliding, view.sliding)

add_executable(view.split split.cpp)
target_link_libraries(view.split range-v3)
add_test(test.view.split, view.split)

add_executable(view.stride stride.cpp)
target_link_libraries(view.stride range-v3)
add_test(test.view.stride, view.stride)

add_executable(view.tail tail.cpp)
target_link_libraries(view.tail range-v3)
add_test(test.view.tail, view.tail)

add_executable(view.take take.cpp)
target_link_libraries(view.take range-v3)
add_test(test.view.take, view.take)

add_executable(view.take_exactly take_exactly.cpp)
target_link_libraries(view.take_exactly range-v3)
add_test(test.view.take_exactly, view.take_exactly)

add_executable(view.take_while take_while.cpp)
target_link_libraries(view.take_while range-v3)
add_test(test.view.take_while, view.take_while)

add_executable(view.tokenize tokenize.cpp)
target_link_libraries(view.tokenize range-v3)
add_test(test.view.tokenize, view.tokenize)

add_executable(view.transform transform.cpp)
target_link_libraries(view.transform range-v3)
add_test(test.view.transform, view.transform)

add_executable(view.unique unique.cpp)
target_link_libraries(view.unique range-v3)
add_test(test.view.unique, view.unique)

add_executable(view.zip zip.cpp)
target_link_libraries(view.zip range-v3)
add_test(test.view.zip, view.zip)
