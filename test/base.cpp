/// \file
// Range v3 library
//
//  Copyright Andrey Diduh 2019
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#include <vector>
#include <iostream>

#include <range/v3/base.hpp>
#include <range/v3/view/transform.hpp>
#include <range/v3/view/drop.hpp>
#include <range/v3/view/take_exactly.hpp>
#include <range/v3/view/const.hpp>

#include "./simple_test.hpp"
#include "./test_utils.hpp"

using namespace ranges;

struct Data
{
    int i;

    bool operator==(const Data& other) const
    {
        return i == other.i;
    }
};
using Vec = std::vector<Data>;
using Iter = ranges::iterator_t<Vec>;

void test_base_n()
{
    Vec vec = { {1}, {2}, {3}, {4} };
    auto list = vec | view::transform(&Data::i) | view::const_;

    // ITERATOR
    // base()
    {
        check_equal( list.begin().base(), ranges::base(list.begin()) );
    }
    // base<N>()
    {
        check_equal( list.begin().base(), ranges::base<1>(list.begin()) );
        check_equal( list.begin().base().base(), ranges::base<2>(list.begin()) );
        check_equal( list.begin().base().base(), ranges::base(ranges::base(list.begin())) );
        check_equal( vec.begin(), ranges::base<2>(list.begin()) );
    }

    // RANGE
    // base()
    {
        check_equal( list.base(), ranges::base(list) );
    }
    // base<N>()
    {
        check_equal( list.base(), ranges::base<1>(list) );
        check_equal( list.base().base(), ranges::base<2>(list) );
        check_equal( list.base().base(), ranges::base(ranges::base(list)) );
        check_equal( vec, ranges::base<2>(list) );
    }
}

void test_base_of()
{
    Vec vec = { {1}, {2}, {3}, {4} };

    auto repeat = [](const int& i) -> const int& { return i; };

    auto list  = vec | view::transform(&Data::i) | view::take_exactly(3);
    auto list2 = list | view::transform(repeat) | view::transform(repeat) | view::transform(repeat);

    // Not every view introduce new iterator, so:
    // list.begin().base().base() != vec.begin()

    // ITERATOR
    {
        check_equal( vec.begin(), ranges::base<Iter>(list.begin()) );
        check_equal( vec.begin(), ranges::base<Iter>(list2.begin()) );
        check_equal( list.begin(), ranges::base<decltype(list.begin())>(list2.begin()) );

        // 0-base
        check_equal( list.begin(), ranges::base<decltype(list.begin())>(list.begin()) );
        check_equal( vec.begin(), ranges::base<Vec>(list.begin()) );
    }

    // RANGE
    {
        check_equal( vec, ranges::base<Vec>(list) );
        check_equal( vec, ranges::base<Vec>(list2) );
        check_equal( list, ranges::base<decltype(list)>(list2) );

        // 0-base
        check_equal( list, ranges::base<decltype(list)>(list) );
    }
}

int main()
{
    test_base_n();
    test_base_of();

    return test_result();
}
