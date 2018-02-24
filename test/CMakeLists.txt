include(../cmake/ranges_diagnostics.cmake)

add_subdirectory(action)
add_subdirectory(algorithm)
add_subdirectory(numeric)
add_subdirectory(utility)
add_subdirectory(view)
add_subdirectory(experimental)

add_executable(config config.cpp)
target_link_libraries(config range-v3)
add_test(test.config config)

add_executable(container_conversion container_conversion.cpp)
target_link_libraries(container_conversion range-v3)
add_test(test.container_conversion container_conversion)

add_executable(constexpr_core constexpr_core.cpp)
target_link_libraries(constexpr_core range-v3)
add_test(test.constexpr_core constexpr_core)

add_executable(view_facade view_facade.cpp)
target_link_libraries(view_facade range-v3)
add_test(test.view_facade view_facade)

add_executable(view_adaptor view_adaptor.cpp)
target_link_libraries(view_adaptor range-v3)
add_test(test.view_adaptor view_adaptor)

add_executable(index index.cpp)
target_link_libraries(index range-v3)
add_test(test.index index)

add_executable(iterator_range iterator_range.cpp)
target_link_libraries(iterator_range range-v3)
add_test(test.iterator_range iterator_range)

add_executable(multiple multiple1.cpp multiple2.cpp)
target_link_libraries(multiple range-v3)
add_test(test.multiple multiple)

add_executable(distance distance.cpp)
target_link_libraries(distance range-v3)
add_test(test.distance distance)

add_executable(to_container to_container.cpp)
target_link_libraries(to_container range-v3)
add_test(test.to_container to_container)

add_executable(getlines getlines.cpp)
target_link_libraries(getlines range-v3)
add_test(test.getlines getlines)

add_executable(istream_range istream_range.cpp)
target_link_libraries(istream_range range-v3)
add_test(test.istream_range istream_range)

add_executable(bug474 bug474.cpp)
target_link_libraries(bug474 range-v3)
add_test(test.bug474 bug474)

add_executable(bug566 bug566.cpp)
target_link_libraries(bug566 range-v3)
add_test(test.bug566 bug566)

add_executable(span span.cpp)
target_link_libraries(span range-v3)
add_test(test.span span)
