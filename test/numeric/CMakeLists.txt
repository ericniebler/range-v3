
cxx_test(
  PRETTY_NAME test.num.accumulate
  BIN_NAME num.accumulate
  SOURCES accumulate.cpp
)

cxx_test(
  PRETTY_NAME test.num.adjacent_difference
  BIN_NAME num.adjacent_difference
  SOURCES adjacent_difference.cpp
)

cxx_test(
  PRETTY_NAME test.num.inner_product
  BIN_NAME num.inner_product
  SOURCES inner_product.cpp
)

cxx_test(
  PRETTY_NAME test.num.iota
  BIN_NAME num.iota
  SOURCES iota.cpp
)

cxx_test(
  PRETTY_NAME test.num.partial_sum
  BIN_NAME num.partial_sum
  SOURCES partial_sum.cpp
)
