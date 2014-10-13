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
#include <range/v3/view/const.hpp>
#include <range/v3/view/counted.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"
#include "../test_iterators.hpp"

int main()
{
    using namespace ranges;

    int rgi[] = {1, 2, 3, 4};

    auto && rng = rgi | view::const_;
    has_type<int &>(*begin(rgi));
    has_type<int const &>(*begin(rng));
    models<concepts::BoundedRange>(rng);
    models<concepts::SizedRange>(rng);
    models<concepts::RandomAccessRange>(rng);
    ::check_equal(rng, {1, 2, 3, 4});
    CHECK(&*begin(rng) == &rgi[0]);
    CHECK(rng.size() == 4u);

    auto && rng2 = view::counted(forward_iterator<int*>(rgi), 4) | view::const_;
    has_type<int const &>(*begin(rng2));
    models<concepts::ForwardRange>(rng2);
    models_not<concepts::BidirectionalRange>(rng2);
    models_not<concepts::BoundedRange>(rng2);
    models<concepts::SizedRange>(rng2);
    ::check_equal(rng2, {1, 2, 3, 4});
    CHECK(&*begin(rng2) == &rgi[0]);
    CHECK(rng2.size() == 4u);

    return test_result();
}
