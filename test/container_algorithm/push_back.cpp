//  Copyright Gonzalo Brito Gadeschi 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/

#include <cassert>
#include <vector>
#include <list>
#include <deque>
#include <string>
#include <range/v3/algorithm/equal.hpp>
#include <range/v3/container_algorithm/push_back.hpp>

template<class CharContainer> void test() {
  {
    CharContainer c1 {'0', '2', '4', '6'};
    CharContainer c2;
    assert(!ranges::equal(c1, c2));

    ranges::push_back(c2, c1);
    assert(ranges::equal(c1, c2));

    CharContainer ext { '7' };
    ranges::push_back(c2, ext);
    assert(!ranges::equal(c1, c2));

    ranges::push_back(c1, ext);
    assert(ranges::equal(c1, c2));
  }

  {
    CharContainer c1 {'0', '2', '4', '6'};
    CharContainer c2;
    assert(!ranges::equal(c1, c2));

    c2 | ranges::push_back(c1);
    assert(ranges::equal(c1, c2));

    CharContainer ext { '7' };
    c2 | ranges::push_back(ext);
    assert(!ranges::equal(c1, c2));

    c1 | ranges::push_back(ext);
    assert(ranges::equal(c1, c2));
  }
}

int main()
{
  test<std::vector<char>>();
  test<std::list<char>>();
  test<std::deque<char>>();
  test<std::string>();
}
