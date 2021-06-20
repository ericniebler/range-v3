// Range v3 library
//
//  Copyright Semir Vrana 2020-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3

#include <range/v3/core.hpp>
#include <range/v3/view/drop_exactly.hpp>
#include <range/v3/view/successor.hpp>
#include <range/v3/view/take_exactly.hpp>

#include "../simple_test.hpp"
#include "../test_utils.hpp"

namespace views = ranges::views;

int main()
{
    {
        auto powers_of_10 = views::successor([](int x) { return x * 10; }, 1);
        CPP_assert(ranges::input_range<decltype(powers_of_10)> &&
                   ranges::view_<decltype(powers_of_10)>);
        check_equal(powers_of_10 | views::take_exactly(5), {1, 10, 100, 1000, 10000});
    }

    // Test that we only call the function once for each dereferenceable position
    {
        int i = 0;
        auto rng = views::successor(
            [&i](int n) {
                ++i;
                return n + 1;
            },
            0);
        auto rng2 = std::move(rng);
        auto it = rng2.begin();
        CHECK(i == 0);
        CHECK(*it == 0);
        CHECK(i == 0);
        ++it;
        CHECK(i == 1);
        CHECK(*it == 1);
        CHECK(i == 1);
    }

    // Test that skipping past positions works correctly
    {
        auto times_10 = [](int x) { return x * 10; };
        auto rng = ranges::views::successor(times_10, 1) //
                   | ranges::views::drop_exactly(3)      //
                   | ranges::views::take_exactly(2);

        check_equal(rng, {1000, 10000});
    }

    return test_result();
}
