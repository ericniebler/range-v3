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
#include <range/v3/view/sliding.hpp>
#include <range/v3/view/reverse.hpp>
#include <range/v3/view/repeat.hpp>
#include <range/v3/view/cycle.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"

int main()
{
    using namespace ranges;

    {
        std::vector<int> v = view::iota(0,7);
        auto rng = v | view::sliding(3);
        ::models<concepts::RandomAccessRange>(rng);
        ::models<concepts::SizedRange>(rng);
        auto it = ranges::begin(rng);
        ::check_equal(*it++, {0,1,2});
        ::check_equal(*it++, {1,2,3});
        ::check_equal(*it++, {2,3,4});
        ::check_equal(*it++, {3,4,5});
        ::check_equal(*it++, {4,5,6});
        ::check_equal(*it++, {5,6});
        ::check_equal(*it++, {6});
        CHECK(it == ranges::end(rng));
        ::check_equal(*ranges::next(it, -3), {4,5,6});
        CHECK(size(rng), 7u);
        if (!ranges::v3::detail::broken_ebo)
            CHECK(sizeof(rng.begin()) == sizeof(v.begin()) * 2 + sizeof(std::ptrdiff_t));
    }

    {
        std::forward_list<int> l = view::iota(0,7);
        auto rng = l | view::sliding(3);
        ::models<concepts::ForwardRange>(rng);
        ::models_not<concepts::BidirectionalRange>(rng);
        ::models_not<concepts::SizedRange>(rng);
        auto it = ranges::begin(rng);
        ::check_equal(*it++, {0,1,2});
        ::check_equal(*it++, {1,2,3});
        ::check_equal(*it++, {2,3,4});
        ::check_equal(*it++, {3,4,5});
        ::check_equal(*it++, {4,5,6});
        ::check_equal(*it++, {5,6});
        ::check_equal(*it++, {6});
        CHECK(it == ranges::end(rng));
        if (!ranges::v3::detail::broken_ebo)
            CHECK(sizeof(rng.begin()) == sizeof(l.begin()) * 2 + sizeof(std::ptrdiff_t));
    }

    {
        // An infinite, cyclic range with cycle length == 1
        auto fives = view::repeat(5);
        ::models<concepts::RandomAccessRange>(fives);
        auto rng = fives | view::sliding(3);
        auto it = rng.begin();
        using It = decltype(it);
        static_assert(RandomAccessIterator<It>(), "");
        auto it2 = next(it,5);
        CHECK((it2 - it) == 0);
        ::check_equal(*it, {5,5,5});
        ::check_equal(*it2, {5,5,5});
    }

    {
        // An infinite, cyclic range with cycle length == 3
        std::initializer_list<int> ints = {0,1,2};
        auto cyc = ints | view::cycle;
        //[0,1],[1,2],[2,0],[0,1],[1,2],[2,0],
        auto rng = cyc | view::sliding(2);
        auto it = rng.begin();
        using It = decltype(it);
        static_assert(RandomAccessIterator<It>(), "");
        auto it2 = next(it,2);
        ::check_equal(*it, {0,1});
        ::check_equal(*it2, {2,0});

        CHECK((it - it) == 0);
        CHECK((next(it) - it) == 1);
        CHECK((next(it,2) - it) == 2);
        CHECK((next(it,3) - it) == 0);
        CHECK((next(it,4) - it) == 1);
        CHECK((next(it,5) - it) == 2);
        CHECK((next(it,6) - it) == 0);
        CHECK((next(it,7) - it) == 1);
    }

    {
        // An infinite, cyclic range with cycle length == 3
        std::initializer_list<int> ints = {0,1,2};
        auto cyc = ints | view::cycle;
        //[0,1,2,0],[1,2,0,1],[2,0,1,2],...
        auto rng = cyc | view::sliding(4);
        auto it = rng.begin();
        using It = decltype(it);
        static_assert(RandomAccessIterator<It>(), "");
        auto it2 = next(it,2);
        ::check_equal(*it, {0,1,2,0});
        ::check_equal(*it2, {2,0,1,2});

        CHECK((it - it) == 0);
        CHECK((next(it) - it) == 1);
        CHECK((next(it,2) - it) == 2);
        CHECK((next(it,3) - it) == 0);
        CHECK((next(it,4) - it) == 1);
        CHECK((next(it,5) - it) == 2);
        CHECK((next(it,6) - it) == 0);
        CHECK((next(it,7) - it) == 1);
    }

    {
        // An infinite, cyclic range with cycle length == 10
        std::initializer_list<int> ints = {0,1,2,3,4,5,6,7,8,9};
        auto cyc = ints | view::cycle;
        auto rng = cyc | view::sliding(3);
        //[0,1,2],[3,4,5],[6,7,8],[9,0,1],[2,3,4],...
        auto it = rng.begin();
        using It = decltype(it);
        static_assert(RandomAccessIterator<It>(), "");
        ::check_equal(*it, {0,1,2});
        ::check_equal(*next(it, 2), {2,3,4});
        ::check_equal(*next(it,12), {2,3,4});
        ::check_equal(*next(it,19), {9,0,1});

        CHECK((it - it) == 0);
        CHECK((next(it) - it) == 1);
        CHECK((next(it,2) - it) == 2);
        CHECK((next(it,3) - it) == 3);
        CHECK((next(it,4) - it) == 4);
        CHECK((next(it,5) - it) == 5);
        CHECK((next(it,6) - it) == 6);
        CHECK((next(it,7) - it) == 7);
    }

    return ::test_result();
}
