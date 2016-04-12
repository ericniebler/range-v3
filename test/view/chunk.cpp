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

#include <list>
#include <vector>
#include <forward_list>
#include <range/v3/core.hpp>
#include <range/v3/view/iota.hpp>
#include <range/v3/view/chunk.hpp>
#include <range/v3/view/reverse.hpp>
#include <range/v3/view/repeat.hpp>
#include <range/v3/view/cycle.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"

int main()
{
    using namespace ranges;

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
        auto it = rng.begin();
        using It = decltype(it);
        static_assert(RandomAccessIterator<It>(), "");
        auto it2 = next(it,3);
        CHECK((it2 - it) == 0);
        ::check_equal(*it, {5,5,5});
        ::check_equal(*it2, {5,5,5});
    }

    {
        // An infinite, cyclic range with cycle length == 3
        std::initializer_list<int> ints = {0,1,2};
        auto cyc = ints | view::cycle;
        //[0,1],[2,0],[1,2],[0,1],[2,0],[1,2],
        auto rng = cyc | view::chunk(2);
        auto it = rng.begin();
        using It = decltype(it);
        static_assert(RandomAccessIterator<It>(), "");
        auto it2 = next(it,2);
        ::check_equal(*it, {0,1});
        ::check_equal(*it2, {1,2});
        // Strange, but not wrong necessarily:
        CHECK((it - it) == 0);
        CHECK((next(it) - it) == 1);
        CHECK((next(it,2) - it) == 0);
        CHECK((next(it,3) - it) == 0);
        CHECK((next(it,4) - it) == 1);
        CHECK((next(it,5) - it) == 0);
        CHECK((next(it,6) - it) == 0);
        CHECK((next(it,7) - it) == 1);
    }

    {
        // An infinite, cyclic range with cycle length == 3
        std::initializer_list<int> ints = {0,1,2};
        auto cyc = ints | view::cycle;
        //[0,1,2,0],[1,2,0,1],[2,0,1,2],...
        auto rng = cyc | view::chunk(4);
        auto it = rng.begin();
        using It = decltype(it);
        static_assert(RandomAccessIterator<It>(), "");
        auto it2 = next(it,2);
        ::check_equal(*it, {0,1,2,0});
        ::check_equal(*it2, {2,0,1,2});
        // Strange, but not wrong necessarily:
        CHECK((it - it) == 0);
        CHECK((next(it) - it) == 0);
        CHECK((next(it,2) - it) == 0);
        CHECK((next(it,3) - it) == 0);
        CHECK((next(it,4) - it) == 0);
        CHECK((next(it,5) - it) == 0);
        CHECK((next(it,6) - it) == 0);
        CHECK((next(it,7) - it) == 0);
    }

    {
        // An infinite, cyclic range with cycle length == 10
        std::initializer_list<int> ints = {0,1,2,3,4,5,6,7,8,9};
        auto cyc = ints | view::cycle;
        auto rng = cyc | view::chunk(3);
        //[0,1,2],[3,4,5],[6,7,8],[9,0,1],[2,3,4],...
        auto it = rng.begin();
        using It = decltype(it);
        static_assert(RandomAccessIterator<It>(), "");
        auto it2 = next(it,2);
        ::check_equal(*it, {0,1,2});
        ::check_equal(*it2, {6,7,8});
        // Strange, but not wrong necessarily:
        CHECK((it - it) == 0);
        CHECK((next(it) - it) == 1);
        CHECK((next(it,2) - it) == 2);
        CHECK((next(it,3) - it) == 3);
        CHECK((next(it,4) - it) == 0);
        CHECK((next(it,5) - it) == 1);
        CHECK((next(it,6) - it) == 2);
        CHECK((next(it,7) - it) == 0);
    }

    return ::test_result();
}
