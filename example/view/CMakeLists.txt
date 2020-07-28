set(CMAKE_FOLDER "${CMAKE_FOLDER}/view")

rv3_add_test(example.view.transform example.view.transform transform.cpp)
# NOTE: Make sure the output matches the contents of `transform_golden.txt`
set_tests_properties(range.v3.example.view.transform PROPERTIES PASS_REGULAR_EXPRESSION "\\[0.5,1,1.5\\]")

