
add_executable(indirect indirect.cpp)
add_test(test.indirect indirect)

add_executable(map keys_value.cpp)
add_test(test.map map)

add_executable(replace replace.cpp)
add_test(test.replace replace)

add_executable(replace_if replace_if.cpp)
add_test(test.replace_if replace_if)

add_executable(slice slice.cpp)
add_test(test.slice slice)

add_executable(adjacent_filter adjacent_filter.cpp)
add_test(test.adjacent_filter adjacent_filter)

add_executable(unique unique.cpp)
add_test(test.unique unique)

add_executable(zip zip.cpp)
add_test(test.zip zip)

add_executable(move move.cpp)
add_test(test.move move)

add_executable(delimit delimit.cpp)
add_test(test.delimit delimit)
