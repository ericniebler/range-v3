// Range v3 library
//
//  Copyright Eric Niebler 2014-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <array>
#include <random>
#include <vector>
#include <range/v3/core.hpp>
#include <range/v3/view/iota.hpp>
#include <range/v3/view/stride.hpp>
#include <range/v3/view/take.hpp>
#include <range/v3/algorithm/shuffle.hpp>
#include <range/v3/algorithm/copy.hpp>
#include <range/v3/algorithm/move.hpp>
#include <range/v3/algorithm/is_sorted.hpp>
#include <range/v3/algorithm/equal.hpp>
#include <range/v3/action/shuffle.hpp>
#include <range/v3/action/sort.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"

int main()
{
    using namespace ranges;
    std::mt19937 gen;

    std::vector<int> v = view::ints(0,100);
    v |= action::shuffle(gen);
    CHECK(!is_sorted(v));

    auto v2 = v | copy | action::sort;
    CHECK(size(v2) == size(v));
    CHECK(is_sorted(v2));
    CHECK(!is_sorted(v));
    ::models<concepts::Same>(v, v2);

    v |= action::sort;
    CHECK(is_sorted(v));

    v |= action::shuffle(gen);
    CHECK(!is_sorted(v));

    v = v | move | action::sort(std::less<int>());
    CHECK(is_sorted(v));
    CHECK(equal(v, v2));

    // Container algorithms can also be called directly
    // in which case they take and return by reference
    shuffle(v, gen);
    CHECK(!is_sorted(v));
    auto & v3 = action::sort(v);
    CHECK(is_sorted(v));
    CHECK(&v3 == &v);

    auto ref=std::ref(v);
    ref |= action::sort;

    // Can pipe a view to a "container" algorithm.
    action::sort(v, std::greater<int>());
    v | view::stride(2) | action::sort;
    check_equal(view::take(v, 10), {1,98,3,96,5,94,7,92,9,90});

    return ::test_result();
}
