// Range v3 library
//
//  Copyright Eric Niebler 2014-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <array>
#include <vector>
#include <range/v3/core.hpp>
#include <range/v3/view/iota.hpp>
#include <range/v3/view/stride.hpp>
#include <range/v3/algorithm/copy.hpp>
#include <range/v3/algorithm/move.hpp>
#include <range/v3/algorithm/equal.hpp>
#include <range/v3/action/slice.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"

int main()
{
    using namespace ranges;

    std::vector<int> rng = view::ints(0,100);

    auto rng_copy = rng | copy | action::slice(10,20);
    CHECK(size(rng_copy) == 10u);
    ::models<concepts::Same>(rng, rng_copy);
    ::check_equal(rng_copy, {10,11,12,13,14,15,16,17,18,19});

    std::vector<int> rng_2 = view::ints(0,100);

    rng_2 |= action::slice(20,end-70);
    CHECK(size(rng_2) == 10u);
    ::check_equal(rng_2, {20,21,22,23,24,25,26,27,28,29});

    rng_2 |= action::slice(end-10,end-5);
    CHECK(size(rng_2) == 5u);
    ::check_equal(rng_2, {20,21,22,23,24});

    std::vector<int> rng_3 = view::ints(0,100);

    auto& rng_3_copy = action::slice(rng_3, 90, end);
    CHECK(&rng_3_copy == &rng_3);
    CHECK(size(rng_3_copy) == 10u);
    ::check_equal(rng_3, {90,91,92,93,94,95,96,97,98,99});

    rng_3 |= action::slice(end-5, end);
    CHECK(&rng_3_copy == &rng_3);
    CHECK(size(rng_3_copy) == 5u);
    ::check_equal(rng_3, {95,96,97,98,99});

    return ::test_result();
}
