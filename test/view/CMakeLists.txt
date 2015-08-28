
cxx_test(
  PRETTY_NAME test.view.adjacent_remove_if
  BIN_NAME view.adjacent_remove_if
  SOURCES adjacent_remove_if.cpp
)

cxx_test(
  PRETTY_NAME test.view.all
  BIN_NAME view.all
  SOURCES all.cpp
)

cxx_test(
  PRETTY_NAME test.view.any_view
  BIN_NAME view.any_view
  SOURCES any_view.cpp
)

cxx_test(
  PRETTY_NAME test.view.bounded
  BIN_NAME view.bounded
  SOURCES bounded.cpp
)

# Mysterious Clang bug, not yet investigated:
if(NOT "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" )
  cxx_test(
    PRETTY_NAME test.view.chunk
    BIN_NAME view.chunk
    SOURCES chunk.cpp
  )
endif()

cxx_test(
  PRETTY_NAME test.view.concat
  BIN_NAME view.concat
  SOURCES concat.cpp
)

cxx_test(
  PRETTY_NAME test.view.const
  BIN_NAME view.const
  SOURCES const.cpp
)

cxx_test(
  PRETTY_NAME test.view.counted
  BIN_NAME view.counted
  SOURCES counted.cpp
)

# Mysterious Clang bug, not yet investigated:
if(NOT "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" )
  cxx_test(
    PRETTY_NAME test.view.cycle
    BIN_NAME view.cycle
    SOURCES cycle.cpp
  )
endif()

cxx_test(
  PRETTY_NAME test.view.delimit
  BIN_NAME view.delimit
  SOURCES delimit.cpp
)

cxx_test(
  PRETTY_NAME test.view.drop
  BIN_NAME view.drop
  SOURCES drop.cpp
)

cxx_test(
  PRETTY_NAME test.view.drop_while
  BIN_NAME view.drop_while
  SOURCES drop_while.cpp
)

cxx_test(
  PRETTY_NAME test.view.generate
  BIN_NAME view.generate
  SOURCES generate.cpp
)

cxx_test(
  PRETTY_NAME test.view.generate_n
  BIN_NAME view.generate_n
  SOURCES generate_n.cpp
)

cxx_test(
  PRETTY_NAME test.view.group_by
  BIN_NAME view.group_by
  SOURCES group_by.cpp
)

cxx_test(
  PRETTY_NAME test.view.indirect
  BIN_NAME view.indirect
  SOURCES indirect.cpp
)

cxx_test(
  PRETTY_NAME test.view.intersperse
  BIN_NAME view.intersperse
  SOURCES intersperse.cpp
)

cxx_test(
  PRETTY_NAME test.view.iota
  BIN_NAME view.iota
  SOURCES iota.cpp
)

cxx_test(
  PRETTY_NAME test.view.join
  BIN_NAME view.join
  SOURCES join.cpp
)

cxx_test(
  PRETTY_NAME test.view.map
  BIN_NAME view.map
  SOURCES keys_value.cpp
)

cxx_test(
  PRETTY_NAME test.view.move
  BIN_NAME view.move
  SOURCES move.cpp
)

cxx_test(
  PRETTY_NAME test.view.partial_sum
  BIN_NAME view.partial_sum
  SOURCES partial_sum.cpp
)

cxx_test(
  PRETTY_NAME test.view.repeat
  BIN_NAME view.repeat
  SOURCES repeat.cpp
)

cxx_test(
  PRETTY_NAME test.view.remove_if
  BIN_NAME view.remove_if
  SOURCES remove_if.cpp
)

cxx_test(
  PRETTY_NAME test.view.replace
  BIN_NAME view.replace
  SOURCES replace.cpp
)

cxx_test(
  PRETTY_NAME test.view.replace_if
  BIN_NAME view.replace_if
  SOURCES replace_if.cpp
)

cxx_test(
  PRETTY_NAME test.view.reverse
  BIN_NAME view.reverse
  SOURCES reverse.cpp
)

cxx_test(
  PRETTY_NAME test.view.slice
  BIN_NAME view.slice
  SOURCES slice.cpp
)

cxx_test(
  PRETTY_NAME test.view.split
  BIN_NAME view.split
  SOURCES split.cpp
)

cxx_test(
  PRETTY_NAME test.view.stride
  BIN_NAME view.stride
  SOURCES stride.cpp
)

cxx_test(
  PRETTY_NAME test.view.take
  BIN_NAME view.take
  SOURCES take.cpp
)

cxx_test(
  PRETTY_NAME test.view.take_exactly
  BIN_NAME view.take_exactly
  SOURCES take_exactly.cpp
)

cxx_test(
  PRETTY_NAME test.view.take_while
  BIN_NAME view.take_while
  SOURCES take_while.cpp
)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  cxx_test(
    PRETTY_NAME test.view.tokenize
    BIN_NAME view.tokenize
    SOURCES tokenize.cpp
  )
endif()

cxx_test(
  PRETTY_NAME test.view.transform
  BIN_NAME view.transform
  SOURCES transform.cpp
)

cxx_test(
  PRETTY_NAME test.view.unique
  BIN_NAME view.unique
  SOURCES unique.cpp
)

cxx_test(
  PRETTY_NAME test.view.zip
  BIN_NAME view.zip
  SOURCES zip.cpp
)

# set_target_properties(
#     view.zip
#     PROPERTIES COMPILE_FLAGS "-std=c++1y")
