// Range v3 library
//
//  Copyright Eric Niebler 2014-present
//  Copyright Michel Morin 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3

#include <forward_list>
#include <list>
#include <vector>
#include <limits>
#include <range/v3/core.hpp>
#include <range/v3/view/iota.hpp>
#include <range/v3/view/take_while.hpp>
#include "./simple_test.hpp"
#include "./test_utils.hpp"

template<typename I, typename S>
void test_iterators(I begin, S end, ranges::difference_type_t<I> n)
{
    using namespace ranges;

    CHECK(distance(begin, end) == n);
    CHECK(distance_compare(begin, end, n) == 0);
    CHECK(distance_compare(begin, end, n - 1) > 0);
    CHECK(distance_compare(begin, end, n + 1) < 0);
    CHECK(distance_compare(begin, end, (std::numeric_limits<difference_type_t<I>>::min)()) > 0);
    CHECK(distance_compare(begin, end, (std::numeric_limits<difference_type_t<I>>::max)()) < 0);
}

template<typename Rng>
void test_range(Rng&& rng, ranges::range_difference_type_t<Rng> n)
{
    using namespace ranges;

    CHECK(distance(rng) == n);
    CHECK(distance_compare(rng, n) == 0);
    CHECK(distance_compare(rng, n - 1) > 0);
    CHECK(distance_compare(rng, n + 1) < 0);
    CHECK(distance_compare(rng, (std::numeric_limits<range_difference_type_t<Rng>>::min)()) > 0);
    CHECK(distance_compare(rng, (std::numeric_limits<range_difference_type_t<Rng>>::max)()) < 0);
}

template<typename Rng>
void test_infinite_range(Rng&& rng)
{
    using namespace ranges;

    CHECK(distance_compare(rng, 0) > 0);
    CHECK(distance_compare(rng,-1) > 0);
    CHECK(distance_compare(rng, 1) > 0);
    CHECK(distance_compare(rng, (std::numeric_limits<range_difference_type_t<Rng>>::min)()) > 0);
    if (is_infinite<Rng>::value) {
        // For infinite ranges that can be detected by is_infinite<Rng> traits,
        // distance_compare can compute the result in constant time.
        CHECK(distance_compare(rng, (std::numeric_limits<range_difference_type_t<Rng>>::max)()) > 0);
    }
    else {
        // For other infinite ranges, comparing to a huge number might take too much time.
        // Thus commented out the test.
        // CHECK(distance_compare(rng, (std::numeric_limits<range_difference_type_t<Rng>>::max)()) > 0);
    }
}

int main()
{
    using namespace ranges;

    {
        using cont_t = std::vector<int>;
        cont_t c {1, 2, 3, 4};
        test_range(c, 4);
        test_iterators(c.begin(), c.end(), 4);

        c.clear();
        test_range(c, 0);
        test_iterators(c.begin(), c.end(), 0);
    }

    {
        using cont_t = std::list<int>;
        cont_t c {1, 2, 3, 4};
        test_range(c, 4);
        test_iterators(c.begin(), c.end(), 4);

        c.clear();
        test_range(c, 0);
        test_iterators(c.begin(), c.end(), 0);
    }

    {
        using cont_t = std::forward_list<int>;
        cont_t c {1, 2, 3, 4};
        test_range(c, 4);
        test_iterators(c.begin(), c.end(), 4);

        c.clear();
        test_range(c, 0);
        test_iterators(c.begin(), c.end(), 0);
    }

    {
        int a[] = {1, 2, 3, 4};
        test_iterators(a + 4, a, -4);
    }

    {
        test_range(view::ints(0) | view::take_while([](int i) { return i < 4; }), 4);
    }

    {
        test_infinite_range(view::ints(0u));
    }

    return ::test_result();
}
