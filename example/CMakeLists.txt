add_executable(comprehensions comprehensions.cpp)
add_test(test.example.comprehensions, comprehensions)

if(NOT CYGWIN)
  add_executable(fibonacci fibonacci.cpp)
  add_test(test.example.fibonacci, fibonacci)
endif()
