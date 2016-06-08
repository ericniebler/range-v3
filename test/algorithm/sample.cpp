// Range v3 lbrary
//
//  Copyright Eric Niebler 2014
//  Copyright Casey Carter 2016
//
//  Use, modification and distrbution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
//  Copyright 2005 - 2007 Adobe Systems Incorporated
//  Distrbuted under the MIT License(see accompanying file LICENSE_1_0_0.txt
//  or a copy at http://stlab.adobe.com/licenses.html)

//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include <array>
#include <range/v3/core.hpp>
#include <range/v3/algorithm/equal.hpp>
#include <range/v3/algorithm/sample.hpp>
#include <range/v3/numeric/iota.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"
#include "../test_iterators.hpp"

int main()
{
    constexpr unsigned N = 100;
    constexpr unsigned K = 10;
    {
        std::array<int, N> i;
        ranges::iota(i, 0);
        std::array<int, K> a{}, b{}, c{};
        std::minstd_rand g1, g2 = g1;
        ranges::sample(random_access_iterator<int*>(i.data()), sentinel<int*>(i.data()+N), a.begin(), K, g1);
        CHECK(!ranges::equal(a, c));

        CHECK(ranges::sample(i.begin(), i.end(), b.begin(), K, g1) == b.end());
        CHECK(!ranges::equal(a, b));
        CHECK(!ranges::equal(b, c));

        CHECK(ranges::sample(i.begin(), i.end(), c.begin(), K, g2) == c.end());
        CHECK(ranges::equal(a, c));
    }

    {
        std::array<int, N> i;
        ranges::iota(i, 0);
        std::array<int, K> a{}, b{}, c{};
        std::minstd_rand g1, g2 = g1;
        auto rng = ranges::make_iterator_range(random_access_iterator<int*>(i.data()), sentinel<int*>(i.data() + N));
        ranges::sample(rng, a.begin(), K, g1);
        CHECK(!ranges::equal(a, b));

        CHECK(ranges::sample(i, b.begin(), K, g2) == b.end());
        CHECK(ranges::equal(a, b));

        CHECK(ranges::sample(i, b.begin(), K, g1) == b.end());
        CHECK(!ranges::equal(a, b));
        CHECK(!ranges::equal(b, c));

        a.fill(0);
        CHECK(ranges::sample(std::move(rng), a.begin(), K, g1) == a.end());
        CHECK(!ranges::equal(a, c));
    }

    {
        std::array<int, N> i;
        ranges::iota(i, 0);
        std::array<int, K> a{}, b{}, c{};
        ranges::sample(random_access_iterator<int*>(i.data()), sentinel<int*>(i.data() + N), a.begin(), K);
        CHECK(!ranges::equal(a, b));

        ranges::sample(i, b.begin(), K);
        CHECK(!ranges::equal(b, c));
        CHECK(!ranges::equal(a, b));
    }

    {
        std::array<MoveOnlyString, 10> source;
        std::array<MoveOnlyString, 4> dest;
        ranges::sample(ranges::make_move_iterator(source.begin()), ranges::make_move_sentinel(source.end()),
            forward_iterator<MoveOnlyString*>(dest.data()), dest.size());
    }

    {
        std::array<int, N> i;
        ranges::iota(i, 0);
        std::array<int, K> a{}, b{}, c{};
        std::minstd_rand g1, g2 = g1;
        ranges::sample(random_access_iterator<int*>(i.data()), sentinel<int*>(i.data()+N), a, g1);
        CHECK(!ranges::equal(a, c));

        CHECK(ranges::sample(i.begin(), i.end(), b, g1) == b.end());
        CHECK(!ranges::equal(a, b));
        CHECK(!ranges::equal(b, c));

        CHECK(ranges::sample(i.begin(), i.end(), c, g2) == c.end());
        CHECK(ranges::equal(a, c));
    }

    {
        std::array<int, N> i;
        ranges::iota(i, 0);
        std::array<int, K> a{}, b{}, c{};
        std::minstd_rand g1, g2 = g1;
        auto rng = ranges::make_iterator_range(random_access_iterator<int*>(i.data()), sentinel<int*>(i.data() + N));
        ranges::sample(rng, a, g1);
        CHECK(!ranges::equal(a, b));

        CHECK(ranges::sample(i, b, g2) == b.end());
        CHECK(ranges::equal(a, b));

        CHECK(ranges::sample(i, b, g1) == b.end());
        CHECK(!ranges::equal(a, b));
        CHECK(!ranges::equal(b, c));

        a.fill(0);
        CHECK(ranges::sample(std::move(rng), a, g1) == a.end());
        CHECK(!ranges::equal(a, c));
    }

    {
        std::array<int, N> i;
        ranges::iota(i, 0);
        std::array<int, K> a{}, b{}, c{};
        ranges::sample(random_access_iterator<int*>(i.data()), sentinel<int*>(i.data() + N), a);
        CHECK(!ranges::equal(a, b));

        ranges::sample(i, b);
        CHECK(!ranges::equal(b, c));
        CHECK(!ranges::equal(a, b));
    }

    {
        std::array<MoveOnlyString, 10> source;
        std::array<MoveOnlyString, 4> dest;
        auto out = ranges::make_iterator_range(
            forward_iterator<MoveOnlyString*>(dest.data()),
            sentinel<MoveOnlyString*, true>(dest.data() + dest.size()));
        ranges::sample(ranges::make_move_iterator(source.begin()),
            ranges::make_move_sentinel(source.end()), out);
    }

    {
        int data[] = {0,1,2,3};
        int sample[2];
        std::minstd_rand g;
        ranges::sample(data, sample, g);
        ranges::sample(data, sample);
    }

    return ::test_result();
}
