add_executable(comprehensions comprehensions.cpp)
add_test(test.example.comprehensions, comprehensions)

add_executable(fibonacci fibonacci.cpp)
add_test(test.example.fibonacci, fibonacci)

#add_executable(n_queens n_queens.cpp)
#add_test(test.example.n_queens, n_queens)
#set_target_properties(n_queens PROPERTIES COMPILE_FLAGS "-std=gnu++14")

# Guarded with a variable because:
#  (a) The calendar example causes gcc to puke, and
#  (b) It requires a fix for Boost.Range(!!!) that is
#      currently only available on the develop branch.
if(RANGES_BUILD_CALENDAR_EXAMPLE)
    set(Boost_USE_STATIC_LIBS        ON)
    set(Boost_USE_MULTITHREADED     OFF)
    set(Boost_USE_STATIC_RUNTIME    OFF)
    find_package(Boost COMPONENTS date_time program_options)

    if (Boost_FOUND)
        include_directories(SYSTEM ${Boost_INCLUDE_DIRS})
        add_executable(calendar calendar.cpp)
        set_target_properties(calendar PROPERTIES COMPILE_FLAGS "-Wno-disabled-macro-expansion -std=gnu++14")
        target_link_libraries(calendar ${Boost_LIBRARIES})
    endif()
endif()
