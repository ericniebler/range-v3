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

#include <vector>
#include <range/v3/core.hpp>
#include <range/v3/view/take.hpp>
#include <range/v3/view/repeat.hpp>
#include <range/v3/algorithm/equal.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"

int main()
{
    using namespace ranges;
    auto rng = view::repeat(9) | view::take(10);
    ::models<concepts::SizedRange>(rng);
    ::models<concepts::Range>(rng);
    ::models_not<concepts::BoundedRange>(rng);
    ::models<concepts::InputIterator>(rng.begin());
    ::models_not<concepts::ForwardIterator>(rng.begin());
    ::check_equal(rng, {9, 9, 9, 9, 9, 9, 9, 9, 9, 9});

#ifdef RANGES_CXX_GREATER_THAN_11
    {
        constexpr auto rng = view::repeat(9) | view::take(10);
        static_assert(ranges::equal(rng, {9, 9, 9, 9, 9, 9, 9, 9, 9, 9}), "");
    }
#endif

    return test_result();
}
