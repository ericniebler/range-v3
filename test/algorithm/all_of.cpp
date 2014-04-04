//  Copyright Andrew Sutton 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/

#include <vector>
#include <range/v3/core.hpp>
#include <range/v3/algorithm/all_of.hpp>
#include "../simple_test.hpp"

bool even(int n) { return n % 2 == 0; }

struct S {
  S(bool p) : test(p) { }

  bool p() const { return test; }

  bool test;
};

int main() 
{
  std::vector<int> v1 { 0, 2, 4, 6 };
  CHECK(ranges::all_of(v1.begin(), v1.end(), even));
  CHECK(ranges::all_of(v1, even));
  CHECK(ranges::all_of({0, 2, 4, 6}, even));

  std::vector<S> v2 { true, true, true };
  CHECK(ranges::all_of(v2.begin(), v2.end(), &S::p));
  CHECK(ranges::all_of(v2, &S::p));
  CHECK(ranges::all_of({S(true)}, &S::p));

  return ::test_result();
}
