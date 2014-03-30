//  Copyright Andrew Sutton 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/

#include <cassert>
#include <range/v3/algorithm/all_of.hpp>

template<typename P, typename T, typename... Args>
bool check(ranges::predicate<P> pred, const T& t, Args&&... args) 
{
  return pred(t, std::forward<Args>(args)...);
}

template<typename P, typename T, typename... Args>
bool check(P pred, const T& t, Args&&... args) 
{
  return check(ranges::predicate<P>(pred), t, std::forward<Args>(args)...);
}

bool even(int n) { return n % 2 == 0; }

struct S {
  bool p1() const { return true; }
  bool p2(int n) const { return n == 0; }

  bool b;
};

int main() 
{
  assert(check(even, 0));
  assert(check(&S::p1, S{}));
  assert(check(&S::p2, S{}, 0));
  
  // TODO: Should this work?
  // check(&S::b, S{}); 
}
