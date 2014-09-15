//  Copyright Gonzalo Brito Gadeschi 2014.
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/

#include <vector>
#include <list>
#include <deque>
#include <range/v3/algorithm/equal.hpp>
#include <range/v3/extension/insert.hpp>
#include "../simple_test.hpp"

template <class Container> void test()
{
    Container r{1, 2, 3, 4, 5, 6, 7, 8};

    // push_back:
    {
        Container a{1, 2, 3, 4};
        Container b{5, 6, 7, 8};
        ranges::insert(a, ranges::end(a), b);
        CHECK(ranges::equal(a, r));
    }

    // push_front:
    {
        Container a{1, 2, 3, 4};
        Container b{5, 6, 7, 8};
        ranges::insert(b, ranges::begin(b), a);
        CHECK(ranges::equal(b, r));
    }

    // insert middle:
    {
        Container a{1, 2, 7, 8};
        Container b{3, 4, 5, 6};
        ranges::insert(a, std::next(ranges::begin(a), 2), b);
        CHECK(ranges::equal(a, r));
    }
}

int main()
{
    test<std::vector<int>>();
    test<std::list<int>>();
    test<std::deque<int>>();

    return ::test_result();
}
