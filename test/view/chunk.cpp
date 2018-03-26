// Range v3 library
//
//  Copyright Eric Niebler 2014-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3

#include <iostream>
#include <forward_list>
#include <list>
#include <vector>
#include <range/v3/core.hpp>
#include <range/v3/view/chunk.hpp>
#include <range/v3/view/cycle.hpp>
#include <range/v3/view/filter.hpp>
#include <range/v3/view/iota.hpp>
#include <range/v3/view/join.hpp>
#include <range/v3/view/move.hpp>
#include <range/v3/view/repeat.hpp>
#include <range/v3/view/reverse.hpp>
#include "../simple_test.hpp"
#include "../test_iterators.hpp"
#include "../test_utils.hpp"

using namespace ranges;

namespace
{
    void test_input_ranges()
    {
        int const ints[] = {0,1,2,3,4};
        static constexpr auto N = ranges::size(ints);
        static constexpr auto K = 2;
        auto make_range = [&]{ return debug_input_view<int const>{ints} | view::chunk(K); };
        auto rng = make_range();
        using Rng = decltype(rng);
        CONCEPT_ASSERT(InputView<Rng>());
        CONCEPT_ASSERT(!ForwardRange<Rng>());
        CONCEPT_ASSERT(SizedRange<Rng>());
        CHECK(ranges::size(rng) == (N + K - 1) / K);
        CONCEPT_ASSERT(SizedSentinel<sentinel_t<Rng>, iterator_t<Rng>>());
        CHECK((ranges::end(rng) - ranges::begin(rng)) == int((N + K - 1) / K));

        rng = make_range();
        auto i = ranges::begin(rng);
        auto e = ranges::end(rng);
        CHECK(i != e);
        if (i == e) return;
        {
            auto r = *i;
            CHECK(ranges::size(r) == 2u);
            auto ii = ranges::begin(r);
            auto ee = ranges::end(r);
            CHECK(ii != ee);
            if (ii == ee) return;
            CHECK((ee - ii) == 2);
            CHECK(*ii == 0);
            CHECK(++ii != ee);
            if (ii == ee) return;
            CHECK((ee - ii) == 1);
            CHECK(*ii == 1);
            CHECK(++ii == ee);
            CHECK((ee - ii) == 0);
        }
        CHECK(++i != e);
        if (i == e) return;
        ::check_equal(*i, {2,3});
        CHECK(++i != e);
        if (i != e)
        {
            ::check_equal(*i, {4});
            CHECK(++i == e);
        }
    }
}

int main()
{
    std::vector<int> v = view::iota(0,11);
    auto rng1 = v | view::chunk(3);
    ::models<concepts::RandomAccessRange>(rng1);
    ::models<concepts::SizedRange>(rng1);
    auto it1 = ranges::begin(rng1);
    ::check_equal(*it1++, {0,1,2});
    ::check_equal(*it1++, {3,4,5});
    ::check_equal(*it1++, {6,7,8});
    ::check_equal(*it1++, {9,10});
    CHECK(it1 == ranges::end(rng1));
    ::check_equal(*ranges::next(it1, -3), {3,4,5});
    CHECK(size(rng1), 4u);
    if (!ranges::v3::detail::broken_ebo)
        CHECK(sizeof(rng1.begin()) == sizeof(v.begin()) * 2 + sizeof(std::ptrdiff_t) * 2);

    std::forward_list<int> l = view::iota(0,11);
    auto rng2 = l | view::chunk(3);
    ::models<concepts::ForwardRange>(rng2);
    ::models_not<concepts::BidirectionalRange>(rng2);
    ::models_not<concepts::SizedRange>(rng2);
    auto it2 = ranges::begin(rng2);
    ::check_equal(*it2++, {0,1,2});
    ::check_equal(*it2++, {3,4,5});
    ::check_equal(*it2++, {6,7,8});
    ::check_equal(*it2++, {9,10});
    CHECK(it2 == ranges::end(rng2));
    if (!ranges::v3::detail::broken_ebo)
        CHECK(sizeof(rng2.begin()) == sizeof(l.begin()) * 2 + sizeof(std::ptrdiff_t));

    {
        // An infinite, cyclic range with cycle length == 1
        auto fives = view::repeat(5);
        ::models<concepts::RandomAccessRange>(fives);
        auto rng = fives | view::chunk(3);
        ::models<concepts::RandomAccessRange>(rng);
        auto it = rng.begin();
        auto it2 = next(it,3);
        CHECK((it2 - it) == 0);
        ::check_equal(*it, {5,5,5});
        ::check_equal(*it2, {5,5,5});
    }

    {
        // An infinite, cyclic range with cycle length == 3
        int const ints[] = {0,1,2};
        auto cyc = ints | view::cycle;
        //[0,1],[2,0],[1,2],[0,1],[2,0],[1,2],
        auto rng = cyc | view::chunk(2);
        ::models<concepts::RandomAccessRange>(rng);
        auto it = rng.begin();
        auto it2 = next(it,2);
        ::check_equal(*it, {0,1});
        ::check_equal(*it2, {1,2});
        // Strange, but not wrong necessarily:
        CHECK((it - it) == 0);
        CHECK((next(it,1) - it) == 1);
        // static_cast<void>(next(it,2) - it); // not in the domain of -
        CHECK((next(it,3) - it) == 0);
        CHECK((next(it,4) - it) == 1);
        // static_cast<void>(next(it,5) - it); // not in the domain of -
        CHECK((next(it,6) - it) == 0);
        CHECK((next(it,7) - it) == 1);
    }

    {
        // An infinite, cyclic range with cycle length == 3
        int const ints[] = {0,1,2};
        auto cyc = ints | view::cycle;
        //[0,1,2,0],[1,2,0,1],[2,0,1,2],...
        auto rng = cyc | view::chunk(4);
        ::models<concepts::RandomAccessRange>(rng);
        auto it = rng.begin();
        auto it2 = next(it,2);
        ::check_equal(*it, {0,1,2,0});
        ::check_equal(*it2, {2,0,1,2});
        // Strange, but not wrong necessarily:
        CHECK((it - it) == 0);
        // static_cast<void>(next(it,1) - it); // not in the domain of -
        // static_cast<void>(next(it,2) - it); // not in the domain of -
        CHECK((next(it,3) - it) == 0);
        // static_cast<void>(next(it,4) - it); // not in the domain of -
        // static_cast<void>(next(it,5) - it); // not in the domain of -
        CHECK((next(it,6) - it) == 0);
        // static_cast<void>(next(it,7) - it); // not in the domain of -
    }

    {
        // An infinite, cyclic range with cycle length == 10
        int const ints[] = {0,1,2,3,4,5,6,7,8,9};
        auto cyc = ints | view::cycle;
        auto rng = cyc | view::chunk(3);
        ::models<concepts::RandomAccessRange>(rng);
        //[0,1,2],[3,4,5],[6,7,8],[9,0,1],[2,3,4],...
        auto it = rng.begin();
        auto it2 = next(it,2);
        ::check_equal(*it, {0,1,2});
        ::check_equal(*it2, {6,7,8});
        // Strange, but not wrong necessarily:
        CHECK((it - it) == 0);
        CHECK((next(it,1) - it) == 1);
        CHECK((next(it,2) - it) == 2);
        CHECK((next(it,3) - it) == 3);
        // static_cast<void>(next(it,4) - it); // not in the domain of -
        // static_cast<void>(next(it,5) - it); // not in the domain of -
        // ...
        // static_cast<void>(next(it,8) - it); // not in the domain of -
        // static_cast<void>(next(it,9) - it); // not in the domain of -
        CHECK((next(it,10) - it) == 0);
        CHECK((next(it,11) - it) == 1);
        CHECK((next(it,12) - it) == 2);
        CHECK((next(it,13) - it) == 3);
    }

    test_input_ranges();

    {
        // Regression test for #567
        std::vector<std::vector<int>> data{{1, 2, 3}, {4, 5, 6}};
        auto rng = data | view::join | view::chunk(2);
        ::models<concepts::InputRange>(rng);
        CONCEPT_ASSERT(InputRange<range_reference_t<decltype(rng)>>());
        int const expected[][2] = {{1, 2}, {3, 4}, {5, 6}};
        ::check_equal(rng, expected);
    }

    {
        // Regression test for not-exactly #567 (https://github.com/ericniebler/range-v3/issues/567#issuecomment-315148392)
        int some_ints[] = {0,1,2,3};
        int const expected[][2] = {{0, 1}, {2, 3}};
        auto rng = view::all(some_ints);
        ::check_equal(rng | view::chunk(2), expected);
    }

    {
        // Regression test for https://stackoverflow.com/questions/49210190
        auto rng = view::closed_iota(1,25)
            | view::filter([](int item){ return item % 10 != 0; })
            | view::chunk(10);
        auto it = ranges::begin(rng);
        auto last = ranges::end(rng);
        CHECK(it != last);
        ::check_equal(*it, {1,2,3,4,5,6,7,8,9,11});
        CHECK(++it != last);
        ::check_equal(*it, {12,13,14,15,16,17,18,19,21,22});
        CHECK(++it != last);
        ::check_equal(*it, {23,24,25});
        CHECK(++it == last);
    }

    return ::test_result();
}
