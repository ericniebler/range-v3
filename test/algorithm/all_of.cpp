//  Copyright Andrew Sutton 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/

#include <cassert>
#include <vector>
#include <range/v3/algorithm/all_of.hpp>

bool even(int n) { return n % 2 == 0; }

int main() 
{
  std::vector<int> v { 0, 2, 4, 6 };
  // assert(ranges::all_of(v, even));
}
