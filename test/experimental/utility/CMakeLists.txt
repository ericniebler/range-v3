
add_executable(async_generator async_generator.cpp)
add_test(test.async_generator async_generator)

add_executable(generator generator.cpp)
add_test(test.generator generator)

add_executable(task task.cpp)
add_test(test.task task)
