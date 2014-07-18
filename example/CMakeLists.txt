add_executable(comprehensions comprehensions.cpp)

if(NOT (CMAKE_COMPILER_IS_GNUCXX AND CYGWIN))
  add_executable(fibonacci fibonacci.cpp)
endif()
