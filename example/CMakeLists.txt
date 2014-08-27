add_executable(comprehensions comprehensions.cpp)

if(NOT CYGWIN)
  add_executable(fibonacci fibonacci.cpp)
endif()
