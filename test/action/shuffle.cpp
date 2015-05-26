// Range v3 library
//
//  Copyright Filip Matzner 2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <vector>
#include <random>
#include <range/v3/core.hpp>
#include <range/v3/view/iota.hpp>
#include <range/v3/view/stride.hpp>
#include <range/v3/algorithm/copy.hpp>
#include <range/v3/algorithm/move.hpp>
#include <range/v3/algorithm/is_sorted.hpp>
#include <range/v3/algorithm/equal.hpp>
#include <range/v3/algorithm/sort.hpp>
#include <range/v3/action/shuffle.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"

int main()
{
    using namespace ranges;
    std::mt19937 gen;

    // "Ints" view vs. shuffled
    std::vector<int> v = view::ints(0,100);
    auto v2 = v | copy | action::shuffle(gen);
    CHECK(is_sorted(v));
    CHECK(!is_sorted(v2));
    CHECK(size(v2) == size(v));
    ::models<concepts::Same>(v, v2);
    CHECK(!equal(v, v2));

    // "Ints" view vs. shuffled and sorted
    sort(v2);
    CHECK(is_sorted(v2));
    CHECK(equal(v, v2));

    // Shuffled vs. shuffled
    v |= action::shuffle(gen);
    v2 = v2 | move | action::shuffle(gen);
    CHECK(!is_sorted(v));
    CHECK(!is_sorted(v2));
    CHECK(size(v2) == size(v));
    CHECK(!equal(v, v2));

    // Container algorithms can also be called directly
    // in which case they take and return by reference
    v = view::ints(0,100);
    auto & v3 = action::shuffle(v, gen);
    CHECK(!is_sorted(v));
    CHECK(&v3 == &v);

    // Create and shuffle container reference
    v = view::ints(0,100);
    auto ref = std::ref(v);
    ref |= action::shuffle(gen);
    CHECK(!is_sorted(v));

    // Can pipe a view to a "container" algorithm.
    v = view::ints(0,100);
    v | view::stride(2) | action::shuffle(gen);
    CHECK(!is_sorted(v));

    return ::test_result();
}
