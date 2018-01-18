# examples use a less draconian set of compiler warnings
ranges_append_flag(RANGES_HAS_WNO_MISSING_BRACES -Wno-missing-braces)

add_executable(comprehensions comprehensions.cpp)
target_link_libraries(comprehensions range-v3)
add_test(example.comprehensions, comprehensions)

add_executable(hello hello.cpp)
target_link_libraries(hello range-v3)
add_test(example.hello, hello)

add_executable(count count.cpp)
target_link_libraries(count range-v3)
add_test(example.count, count)

add_executable(count_if count_if.cpp)
target_link_libraries(count_if range-v3)
add_test(example.count_if, count_if)

# add_executable(fibonacci fibonacci.cpp)
# target_link_libraries(fibonacci range-v3)
# add_test(example.fibonacci, fibonacci)

# Guarded with a variable because the calendar example causes gcc to puke.
if(RANGES_BUILD_CALENDAR_EXAMPLE)
    set(Boost_USE_STATIC_LIBS        ON)
    set(Boost_USE_MULTITHREADED     OFF)
    set(Boost_USE_STATIC_RUNTIME    OFF)
    find_package(Boost 1.59.0 COMPONENTS date_time program_options)

    if (Boost_FOUND)
        include_directories(SYSTEM ${Boost_INCLUDE_DIRS})
        add_executable(calendar calendar.cpp)
        target_link_libraries(calendar range-v3)
        target_link_libraries(calendar ${Boost_LIBRARIES})
        message ("boost: ${Boost_LIBRARY_DIRS}")
        target_compile_definitions(calendar PUBLIC -DBOOST_NO_AUTO_PTR)
        target_compile_options(calendar PRIVATE -std=gnu++14)
        set_target_properties(calendar PROPERTIES LINK_FLAGS "-L${Boost_LIBRARY_DIRS}")
    endif()
endif()
