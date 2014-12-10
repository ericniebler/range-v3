
add_executable(view.adjacent_filter adjacent_filter.cpp)
add_test(test.view.adjacent_filter, view.adjacent_filter)

add_executable(view.all all.cpp)
add_test(test.view.all, view.all)

add_executable(view.any_range any_range.cpp)
add_test(test.view.any_range, view.any_range)

add_executable(view.bounded bounded.cpp)
add_test(test.view.bounded, view.bounded)

add_executable(view.concat concat.cpp)
add_test(test.view.concat, view.concat)

add_executable(view.const const.cpp)
add_test(test.view.const, view.const)

add_executable(view.counted counted.cpp)
add_test(test.view.counted, view.counted)

add_executable(view.delimit delimit.cpp)
add_test(test.view.delimit, view.delimit)

add_executable(view.drop drop.cpp)
add_test(test.view.drop, view.drop)

add_executable(view.drop_while drop_while.cpp)
add_test(test.view.drop_while, view.drop_while)

add_executable(view.generate generate.cpp)
add_test(test.view.generate, view.generate)

add_executable(view.generate_n generate_n.cpp)
add_test(test.view.generate_n, view.generate_n)

add_executable(view.group_by group_by.cpp)
add_test(test.view.group_by, view.group_by)

add_executable(view.indirect indirect.cpp)
add_test(test.view.indirect, view.indirect)

add_executable(view.intersperse intersperse.cpp)
add_test(test.view.intersperse, view.intersperse)

add_executable(view.iota iota.cpp)
add_test(test.view.iota, view.iota)

add_executable(view.join join.cpp)
add_test(test.view.join, view.join)

add_executable(view.map keys_value.cpp)
add_test(test.view.map, view.map)

add_executable(view.move move.cpp)
add_test(test.view.move, view.move)

add_executable(view.partial_sum partial_sum.cpp)
add_test(test.view.partial_sum, view.partial_sum)

add_executable(view.repeat repeat.cpp)
add_test(test.view.repeat, view.repeat)

add_executable(view.remove_if remove_if.cpp)
add_test(test.view.remove_if, view.remove_if)

add_executable(view.replace replace.cpp)
add_test(test.view.replace, view.replace)

add_executable(view.replace_if replace_if.cpp)
add_test(test.view.replace_if, view.replace_if)

add_executable(view.slice slice.cpp)
add_test(test.view.slice, view.slice)

add_executable(view.split split.cpp)
add_test(test.view.split, view.split)

add_executable(view.stride stride.cpp)
add_test(test.view.stride, view.stride)

add_executable(view.take take.cpp)
add_test(test.view.take, view.take)

add_executable(view.take_while take_while.cpp)
add_test(test.view.take_while, view.take_while)

if(NOT CYGWIN OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    add_executable(view.tokenize tokenize.cpp)
    add_test(test.view.tokenize, view.tokenize)
endif()

add_executable(view.transform transform.cpp)
add_test(test.view.transform, view.transform)

add_executable(view.unique unique.cpp)
add_test(test.view.unique, view.unique)

add_executable(view.zip zip.cpp)
add_test(test.view.zip, view.zip)
