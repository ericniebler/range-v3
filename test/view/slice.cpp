//  Copyright Eric Niebler 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/

#include <list>
#include <vector>
#include <range/v3/core.hpp>
#include <range/v3/view/slice.hpp>
#include <range/v3/view/reverse.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"

int main()
{
    using namespace ranges;

    int rgi[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    auto rng0 = rgi | view::slice(3, 9);
    has_type<int &>(*begin(rng0));
    models<concepts::SizedRange>(rng0);
    models<concepts::RandomAccessIterator>(begin(rng0));
    CHECK(range_equal(rng0, {3, 4, 5, 6, 7, 8}));

    auto rng1 = rng0 | view::reverse;
    has_type<int &>(*begin(rng1));
    models<concepts::SizedRange>(rng1);
    models<concepts::RandomAccessIterator>(begin(rng1));
    CHECK(range_equal(rng1, {8, 7, 6, 5, 4, 3}));

    std::vector<int> v{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    auto rng2 = v | view::slice(3, 9) | view::reverse;
    has_type<int &>(*begin(rng2));
    models<concepts::SizedRange>(rng2);
    models<concepts::RandomAccessIterator>(begin(rng2));
    CHECK(range_equal(rng2, {8, 7, 6, 5, 4, 3}));

    std::list<int> l{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    auto rng3 = l | view::slice(3, 9) | view::reverse;
    has_type<int &>(*begin(rng3));
    models<concepts::SizedRange>(rng3);
    models<concepts::BidirectionalIterator>(begin(rng3));
    models_not<concepts::RandomAccessIterator>(begin(rng3));
    CHECK(range_equal(rng3, {8, 7, 6, 5, 4, 3}));

    return test_result();
}
