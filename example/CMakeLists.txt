set(CMAKE_FOLDER "example")

# examples use a less draconian set of compiler warnings
ranges_append_flag(RANGES_HAS_WNO_MISSING_BRACES -Wno-missing-braces)
ranges_append_flag(RANGES_HAS_WNO_SHORTEN_64_TO_32 -Wno-shorten-64-to-32)
ranges_append_flag(RANGES_HAS_WNO_GLOBAL_CONSTRUCTORS -Wno-global-constructors)

rv3_add_test(example.comprehensions comprehensions comprehensions.cpp)
rv3_add_test(example.hello hello hello.cpp)
rv3_add_test(example.count count count.cpp)
rv3_add_test(example.count_if count_if count_if.cpp)
rv3_add_test(example.any_all_none_of any_all_none_of any_all_none_of.cpp)
rv3_add_test(example.for_each_sequence for_each_sequence for_each_sequence.cpp)
rv3_add_test(example.for_each_assoc for_each_assoc for_each_assoc.cpp)
rv3_add_test(example.is_sorted is_sorted is_sorted.cpp)
rv3_add_test(example.find find find.cpp)
rv3_add_test(example.filter_transform filter_transform filter_transform.cpp)
rv3_add_test(example.accumulate_ints accumulate_ints accumulate_ints.cpp)
rv3_add_test(example.comprehension_conversion comprehension_conversion comprehension_conversion.cpp)
rv3_add_test(example.sort_unique sort_unique sort_unique.cpp)

# Guarded with a variable because the calendar example causes MSVC to puke.
if(RANGES_BUILD_CALENDAR_EXAMPLE)
  add_executable(calendar calendar.cpp)
  target_link_libraries(calendar range-v3)
  if( MSVC )
    target_compile_options(calendar PRIVATE /experimental:preprocessor)
  endif( )
endif()
