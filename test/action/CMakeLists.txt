add_executable(act.concepts cont_concepts.cpp)
target_link_libraries(act.concepts range-v3)
add_test(test.act.concepts act.concepts)

add_executable(act.drop drop.cpp)
target_link_libraries(act.drop range-v3)
add_test(test.act.drop act.drop)

add_executable(act.drop_while drop_while.cpp)
target_link_libraries(act.drop_while range-v3)
add_test(test.act.drop_while act.drop_while)

add_executable(act.insert insert.cpp)
target_link_libraries(act.insert range-v3)
add_test(test.act.insert act.insert)

add_executable(act.join join.cpp)
target_link_libraries(act.join range-v3)
add_test(test.act.join act.join)

add_executable(act.push_front push_front.cpp)
target_link_libraries(act.push_front range-v3)
add_test(test.act.push_front act.push_front)

add_executable(act.push_back push_back.cpp)
target_link_libraries(act.push_back range-v3)
add_test(test.act.push_back act.push_back)

add_executable(act.remove_if remove_if.cpp)
target_link_libraries(act.remove_if range-v3)
add_test(test.act.remove_if act.remove_if)

add_executable(act.reverse reverse.cpp)
target_link_libraries(act.reverse range-v3)
add_test(test.act.reverse act.reverse)

add_executable(act.shuffle shuffle.cpp)
target_link_libraries(act.shuffle range-v3)
add_test(test.act.shuffle act.shuffle)

add_executable(act.slice slice.cpp)
target_link_libraries(act.slice range-v3)
add_test(test.act.slice act.slice)

add_executable(act.sort sort.cpp)
target_link_libraries(act.sort range-v3)
add_test(test.act.sort act.sort)

add_executable(act.split split.cpp)
target_link_libraries(act.split range-v3)
add_test(test.act.split act.split)

add_executable(act.stable_sort stable_sort.cpp)
target_link_libraries(act.stable_sort range-v3)
add_test(test.act.stable_sort act.stable_sort)

add_executable(act.stride stride.cpp)
target_link_libraries(act.stride range-v3)
add_test(test.act.stride act.stride)

add_executable(act.take take.cpp)
target_link_libraries(act.take range-v3)
add_test(test.act.take act.take)

add_executable(act.take_while take_while.cpp)
target_link_libraries(act.take_while range-v3)
add_test(test.act.take_while act.take_while)

add_executable(act.transform transform.cpp)
target_link_libraries(act.transform range-v3)
add_test(test.act.transform act.transform)

add_executable(act.unique unique.cpp)
target_link_libraries(act.unique range-v3)
add_test(test.act.unique act.unique)
