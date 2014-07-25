
add_executable(view.adjacent_filter adjacent_filter.cpp)
add_test(test.view.adjacent_filter, view.adjacent_filter)

add_executable(view.as_range as_range.cpp)
add_test(test.view.as_range, view.as_range)

add_executable(view.counted counted.cpp)
add_test(test.view.counted, view.counted)

add_executable(view.delimit delimit.cpp)
add_test(test.view.delimit, view.delimit)

add_executable(view.generate generate.cpp)
add_test(test.view.generate, view.generate)

add_executable(view.generate_n generate_n.cpp)
add_test(test.view.generate_n, view.generate_n)

add_executable(view.indirect indirect.cpp)
add_test(test.view.indirect, view.indirect)

add_executable(view.join join.cpp)
add_test(test.view.join, view.join)

add_executable(view.map keys_value.cpp)
add_test(test.view.map, view.map)

add_executable(view.move move.cpp)
add_test(test.view.move, view.move)

add_executable(view.repeat repeat.cpp)
add_test(test.view.repeat, view.repeat)

add_executable(view.replace replace.cpp)
add_test(test.view.replace, view.replace)

add_executable(view.replace_if replace_if.cpp)
add_test(test.view.replace_if, view.replace_if)

add_executable(view.slice slice.cpp)
add_test(test.view.slice, view.slice)

add_executable(view.stride stride.cpp)
add_test(test.view.stride, view.stride)

add_executable(view.take_while take_while.cpp)
add_test(test.view.take_while, view.take_while)

add_executable(view.unique unique.cpp)
add_test(test.view.unique, view.unique)

add_executable(view.zip zip.cpp)
add_test(test.view.zip, view.zip)
