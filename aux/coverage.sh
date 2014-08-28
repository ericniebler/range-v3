#!/bin/bash

make -j
ctest
lcov --capture --base-directory $1 --gcov-tool gcov-4.8 --directory . --output-file coverage.info --no-external
lcov --remove coverage.info \*\/test\/\* -o coverage.info
lcov --remove coverage.info \*\/example\/\* -o coverage.info
genhtml coverage.info --output-directory coverage_results --title "Boost.Range-v3 test coverage" --prefix $1 --legend
