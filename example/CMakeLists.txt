add_executable(comprehensions comprehensions.cpp)
add_test(test.example.comprehensions, comprehensions)

# add_executable(fibonacci fibonacci.cpp)
# add_test(test.example.fibonacci, fibonacci)

# Guarded with a variable because the calendar example causes gcc to puke.
if(RANGES_BUILD_CALENDAR_EXAMPLE)
    set(Boost_USE_STATIC_LIBS        ON)
    set(Boost_USE_MULTITHREADED     OFF)
    set(Boost_USE_STATIC_RUNTIME    OFF)
    find_package(Boost 1.59.0 COMPONENTS date_time program_options)

    if (Boost_FOUND)
        include_directories(SYSTEM ${Boost_INCLUDE_DIRS})
        add_executable(calendar calendar.cpp)
        target_link_libraries(calendar ${Boost_LIBRARIES})
        set_target_properties(calendar PROPERTIES COMPILE_FLAGS "-std=gnu++14")
    endif()
endif()
