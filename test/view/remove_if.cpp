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

#include <iterator>
#include <functional>
#include <range/v3/core.hpp>
#include <range/v3/view/remove_if.hpp>
#include <range/v3/view/counted.hpp>
#include <range/v3/view/reverse.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"
#include "../test_iterators.hpp"

struct is_odd
{
    bool operator()(int i) const
    {
        return (i % 2) == 1;
    }
};

struct is_even
{
    bool operator()(int i) const
    {
        return (i % 2) == 0;
    }
};

int main()
{
    using namespace ranges;

    int rgi[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    auto && rng = rgi | view::remove_if(is_even());
    has_type<int &>(*begin(rgi));
    has_type<int &>(*begin(rng));
    models<concepts::BoundedRange>(rng);
    models_not<concepts::SizedRange>(rng);
    models<concepts::BidirectionalRange>(rng);
    models_not<concepts::RandomAccessRange>(rng);
    ::check_equal(rng, {1,3,5,7,9});
    ::check_equal(rng | view::reverse, {9,7,5,3,1});
    CHECK(&*begin(rng | view::reverse) == &rgi[8]);

    auto && rng2 = view::counted(rgi, 10) | view::remove_if(not_(is_odd()));
    has_type<int &>(*begin(rng2));
    models<concepts::BidirectionalRange>(rng2);
    models_not<concepts::RandomAccessRange>(rng2);
    models<concepts::BoundedRange>(rng2);
    models_not<concepts::SizedRange>(rng2);
    ::check_equal(rng2, {1,3,5,7,9});
    CHECK(&*begin(rng2) == &rgi[0]);

    auto && rng3 = view::counted(bidirectional_iterator<int*>{rgi}, 10) | view::remove_if(is_even());
    has_type<int &>(*begin(rng3));
    models<concepts::BidirectionalRange>(rng3);
    models_not<concepts::RandomAccessRange>(rng3);
    models_not<concepts::BoundedRange>(rng3);
    models_not<concepts::SizedRange>(rng3);
    ::check_equal(rng3, {1,3,5,7,9});
    CHECK(&*begin(rng3) == &rgi[0]);
    CHECK(&*prev(next(begin(rng3))) == &rgi[0]);

    // Test remove_if with a mutable lambda
    bool flag = true;
    auto mutable_rng = view::remove_if(rgi, [flag](int) mutable { return flag = !flag;});
    ::check_equal(mutable_rng, {1,3,5,7,9});
    CONCEPT_ASSERT(Range<decltype(mutable_rng)>());
    CONCEPT_ASSERT(!Range<decltype(mutable_rng) const>());

    return test_result();
}
