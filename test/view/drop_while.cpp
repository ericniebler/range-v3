// Range v3 library
//
//  Copyright Eric Niebler 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3

#include <list>
#include <range/v3/core.hpp>
#include <range/v3/view/iota.hpp>
#include <range/v3/view/take.hpp>
#include <range/v3/view/drop_while.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"

int main()
{
    using namespace ranges;
    auto rng0 = view::iota(10) | view::drop_while([](int i) { return i < 25; });
    static_assert(range_cardinality<decltype(rng0)>::value == unknown, "");
    ::models<concepts::RandomAccessView>(rng0);
    ::models_not<concepts::BoundedView>(rng0);
    ::models<concepts::RandomAccessIterator>(rng0.begin());
    auto b = rng0.begin();
    CHECK(*b == 25);
    CHECK(*(b+1) == 26);
    ::check_equal(rng0 | view::take(10), {25, 26, 27, 28, 29, 30, 31, 32, 33, 34});

    std::list<int> vi{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    auto rng1 = vi | view::drop_while([](int i) { return i != 50; });
    static_assert(range_cardinality<decltype(rng1)>::value == ranges::finite, "");
    ::models<concepts::BidirectionalView>(rng1);
    ::models<concepts::BoundedView>(rng1);
    ::models<concepts::BidirectionalIterator>(rng1.begin());
    CHECK(rng1.begin() == rng1.end());

    // Check with a mutable predicate
    int rgi[] = {0,1,2,3,4,5,6,7,8,9};
    int cnt = 0;
    auto mutable_only = view::drop_while(rgi, [cnt](int) mutable { return ++cnt <= 5;});
    ::check_equal(mutable_only, {5,6,7,8,9});
    CONCEPT_ASSERT(View<decltype(mutable_only)>());
    CONCEPT_ASSERT(!View<decltype(mutable_only) const>());

    return test_result();
}
