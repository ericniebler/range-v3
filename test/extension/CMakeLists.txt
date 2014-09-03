add_executable(ext.insert insert.cpp)
add_test(test.ext.insert, ext.insert)

add_executable(ext.push_back push_back.cpp)
add_test(test.ext.push_back, ext.push_back)

add_executable(ext.push_front push_front.cpp)
add_test(test.ext.push_front, ext.push_front)
