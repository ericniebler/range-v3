// Range v3 library
//
//  Copyright Eric Niebler 2014
//  Copyright Michel Morin 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3

#include <array>
#include <forward_list>
#include <list>
#include <vector>
#include <limits>
#include <range/v3/core.hpp>
#include <range/v3/view/iota.hpp>
#include "./simple_test.hpp"
#include "./test_utils.hpp"

template <typename I, typename S>
void test_iterators(I begin, S end, ranges::iterator_difference_t<I> n)
{
    using namespace ranges;

    CHECK(distance(begin, end) == n);
    CHECK(distance_compare(begin, end, n) == 0);
    CHECK(distance_compare(begin, end, n - 1) > 0);
    CHECK(distance_compare(begin, end, n + 1) < 0);
    CHECK(distance_compare(begin, end, (std::numeric_limits<iterator_difference_t<I>>::min)()) > 0);
    CHECK(distance_compare(begin, end, (std::numeric_limits<iterator_difference_t<I>>::max)()) < 0);
}

template <typename Rng>
void test_range(Rng&& rng, ranges::range_difference_t<Rng> n)
{
    using namespace ranges;

    CHECK(distance(rng) == n);
    CHECK(distance_compare(rng, n) == 0);
    CHECK(distance_compare(rng, n - 1) > 0);
    CHECK(distance_compare(rng, n + 1) < 0);
    CHECK(distance_compare(rng, (std::numeric_limits<range_difference_t<Rng>>::min)()) > 0);
    CHECK(distance_compare(rng, (std::numeric_limits<range_difference_t<Rng>>::max)()) < 0);
}

template <typename Rng>
void test_infinite_range(Rng&& rng)
{
    using namespace ranges;

    CHECK(distance_compare(rng, 0) > 0);
    CHECK(distance_compare(rng,-1) > 0);
    CHECK(distance_compare(rng, 1) > 0);
    CHECK(distance_compare(rng, (std::numeric_limits<range_difference_t<Rng>>::min)()) > 0);
    if (is_infinite<Rng>::value) {
        // For infinite ranges that can be detected by is_infinite<Rng> traits,
        // distance_compare can compute the result in constant time.
        CHECK(distance_compare(rng, (std::numeric_limits<range_difference_t<Rng>>::max)()) > 0);
    }
    else {
        // For other infinite ranges, comparing to a huge number might take too much time.
        // Thus commented out the test.
        // CHECK(distance_compare(rng, (std::numeric_limits<range_difference_t<Rng>>::max)()) > 0);
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
        test_infinite_range(view::ints(0u));
    }

#ifdef RANGES_CXX_GREATER_THAN_11
    {
        constexpr auto rng = view::iota(1, 10);
        using Rng = decltype(rng);
        constexpr auto bit = ranges::begin(rng);
        using I = decltype(bit);
        constexpr auto it = bit + 5;
        constexpr auto eit = ranges::end(rng);
        constexpr auto n = ranges::size(rng);
        static_assert(n == 10, "");

        static_assert(distance(bit, eit) == n, "");
        static_assert(distance(it, eit) == 5, "");
        static_assert(distance_compare(bit, eit, n) == 0, "");
        static_assert(distance_compare(bit, eit, n - 1) > 0, "");
        static_assert(distance_compare(bit, eit, n + 1) < 0, "");
        static_assert(distance_compare(bit, eit, (std::numeric_limits<iterator_difference_t<I>>::min)()) > 0, "");
         static_assert(distance_compare(bit, eit, (std::numeric_limits<iterator_difference_t<I>>::max)()) < 0, "");
        static_assert(distance(rng) == n, "");
        static_assert(distance_compare(rng, n) == 0, "");
        static_assert(distance_compare(rng, n - 1) > 0, "");
        static_assert(distance_compare(rng, n + 1) < 0, "");
         static_assert(distance_compare(rng, (std::numeric_limits<range_difference_t<Rng>>::min)()) > 0, "");
         static_assert(distance_compare(rng, (std::numeric_limits<range_difference_t<Rng>>::max)()) < 0, "");

    }
#endif


    return ::test_result();
}
