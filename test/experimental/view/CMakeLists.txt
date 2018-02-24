add_executable(view.shared shared.cpp)
target_link_libraries(view.shared range-v3)
add_test(test.view.shared, view.shared)
