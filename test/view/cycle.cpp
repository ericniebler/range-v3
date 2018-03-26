// Range v3 library
//
//  Copyright Eric Niebler 2013-present
//  Copyright Gonzalo Brito Gadeschi 2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3

#include <list>
#include <array>
#include <memory>
#include <forward_list>
#include <range/v3/range_for.hpp>
#include <range/v3/algorithm/count_if.hpp>
#include <range/v3/view/cycle.hpp>
#include <range/v3/view/take.hpp>
#include <range/v3/view/iota.hpp>
#include <range/v3/view/reverse.hpp>
#include <range/v3/view/c_str.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"

using namespace ranges;

template<typename Rng>
void test_const_forward_range(Rng const &rng)
{
    auto r = rng | view::cycle;
    static_assert(is_infinite<decltype(r)>{}, "");
    static_assert(!BoundedRange<decltype(r)>{}, "");
    CHECK(distance(r | view::take_exactly(0)) == 0);
    CHECK(distance(r | view::take_exactly(1)) == 1);
    CHECK(distance(r | view::take_exactly(2)) == 2);
    CHECK(distance(r | view::take_exactly(3)) == 3);
    CHECK(distance(r | view::take_exactly(4)) == 4);
    CHECK(distance(r | view::take_exactly(6)) == 6);
    CHECK(distance(r | view::take_exactly(7)) == 7);
    CHECK(count_if(r | view::take_exactly(7), [](int) { return true; }) == 7);

    ::check_equal(r | view::take_exactly(0), std::array<int, 0>{});
    ::check_equal(r | view::take_exactly(1), {0});
    ::check_equal(r | view::take_exactly(2), {0, 1});
    ::check_equal(r | view::take_exactly(3), {0, 1, 2});
    ::check_equal(r | view::take_exactly(4), {0, 1, 2, 0});
    ::check_equal(r | view::take_exactly(6), {0, 1, 2, 0, 1, 2});
    ::check_equal(r | view::take_exactly(7), {0, 1, 2, 0, 1, 2, 0});

    CHECK(distance(r | view::take(0)) == 0);
    CHECK(distance(r | view::take(1)) == 1);
    CHECK(distance(r | view::take(2)) == 2);
    CHECK(distance(r | view::take(3)) == 3);
    CHECK(distance(r | view::take(4)) == 4);
    CHECK(distance(r | view::take(6)) == 6);
    CHECK(distance(r | view::take(7)) == 7);
    CHECK(count_if(r | view::take(7), [](int) { return true; }) == 7);

    ::check_equal(r | view::take(0), std::array<int, 0>{});
    ::check_equal(r | view::take(1), {0});
    ::check_equal(r | view::take(2), {0, 1});
    ::check_equal(r | view::take(3), {0, 1, 2});
    ::check_equal(r | view::take(4), {0, 1, 2, 0});
    ::check_equal(r | view::take(6), {0, 1, 2, 0, 1, 2});
    ::check_equal(r | view::take(7), {0, 1, 2, 0, 1, 2, 0});
}

template<typename Rng>
void test_const_forward_reversed_range(Rng const &rng)
{
    test_const_forward_range(rng);

    auto r = rng | view::reverse | view::cycle;
    static_assert(is_infinite<decltype(r)>{}, "");
    static_assert(!BoundedRange<decltype(r)>{}, "");

    CHECK(distance(r | view::take_exactly(0)) == 0);
    CHECK(distance(r | view::take_exactly(1)) == 1);
    CHECK(distance(r | view::take_exactly(2)) == 2);
    CHECK(distance(r | view::take_exactly(3)) == 3);
    CHECK(distance(r | view::take_exactly(4)) == 4);
    CHECK(distance(r | view::take_exactly(6)) == 6);
    CHECK(distance(r | view::take_exactly(7)) == 7);
    CHECK(count_if(r | view::take_exactly(7), [](int) { return true; }) == 7);

    ::check_equal(r | view::take_exactly(0), std::array<int, 0>{});
    ::check_equal(r | view::take_exactly(1), {2});
    ::check_equal(r | view::take_exactly(2), {2, 1});
    ::check_equal(r | view::take_exactly(3), {2, 1, 0});
    ::check_equal(r | view::take_exactly(4), {2, 1, 0, 2});
    ::check_equal(r | view::take_exactly(6), {2, 1, 0, 2, 1, 0});
    ::check_equal(r | view::take_exactly(7), {2, 1, 0, 2, 1, 0, 2});

    CHECK(distance(r | view::take(0)) == 0);
    CHECK(distance(r | view::take(1)) == 1);
    CHECK(distance(r | view::take(2)) == 2);
    CHECK(distance(r | view::take(3)) == 3);
    CHECK(distance(r | view::take(4)) == 4);
    CHECK(distance(r | view::take(6)) == 6);
    CHECK(distance(r | view::take(7)) == 7);
    CHECK(count_if(r | view::take(7), [](int) { return true; }) == 7);

    ::check_equal(r | view::take(0), std::array<int, 0>{});
    ::check_equal(r | view::take(1), {2});
    ::check_equal(r | view::take(2), {2, 1});
    ::check_equal(r | view::take(3), {2, 1, 0});
    ::check_equal(r | view::take(4), {2, 1, 0, 2});
    ::check_equal(r | view::take(6), {2, 1, 0, 2, 1, 0});
    ::check_equal(r | view::take(7), {2, 1, 0, 2, 1, 0, 2});
}

template<typename Rng>
void test_mutable_forward_range_reversed(Rng &rng)
{
    test_const_forward_reversed_range(rng);
    int count = 2;
    RANGES_FOR(auto &&i, rng | view::cycle | view::take_exactly(6)) { i = ++count; }
    ::check_equal(rng | view::take_exactly(3), {6, 7, 8});
}

template<typename Rng>
void test_forward_it(Rng const &rng)
{
    auto r = rng | view::cycle;
    static_assert(ForwardRange<decltype(r)>{}, "");
    auto f = begin(r);
    static_assert(ForwardIterator<decltype(f)>{}, "");

    CHECK((*f) == 0);
    auto n = next(f, 1);
    CHECK((*n) == 1);
}

template<typename Rng>
void test_bidirectional_it(Rng const &rng)
{
    test_forward_it(rng);
    auto r = rng | view::cycle;
    static_assert(BidirectionalRange<decltype(r)>{}, "");
    auto f = begin(r);
    static_assert(BidirectionalIterator<decltype(f)>{}, "");

    CHECK((*f) == 0);
    auto n = next(f, 1);
    auto p = next(f, -1);
    CHECK((*n) == 1);
    CHECK((*p) == 2);
    CHECK((++p) == f);
    CHECK((--n) == f);
}

template<typename Rng>
void test_random_access_it(Rng const &rng)
{
    test_bidirectional_it(rng);
    auto r = rng | view::cycle;
    static_assert(RandomAccessRange<decltype(r)>{}, "");
    auto f = begin(r);
    static_assert(RandomAccessIterator<decltype(f)>{}, "");
    auto m = begin(r) + 1;
    auto l = begin(r) + 2;
    auto f1 = begin(r) + 3;
    auto f2 = begin(r) + 6;

    CHECK(r[0] == 0);
    CHECK(r[1] == 1);
    CHECK(r[2] == 2);
    CHECK(r[3] == 0);
    CHECK(r[4] == 1);
    CHECK(r[5] == 2);
    CHECK(r[-1] == 2);
    CHECK(r[-2] == 1);
    CHECK(r[-3] == 0);
    CHECK(r[-4] == 2);
    CHECK(r[-5] == 1);
    CHECK(r[-6] == 0);

    CHECK((f + 3) == f1);
    CHECK((f + 6) == f2);
    CHECK((f1 + 3) == f2);
    CHECK((f2 - 3) == f1);
    CHECK((f2 - 6) == f);

    auto e = end(r);

    CHECK(*f == 0);
    CHECK(f[0] == 0);
    CHECK(f[1] == 1);
    CHECK(f[2] == 2);
    CHECK(f[3] == 0);
    CHECK(f[4] == 1);
    CHECK(f[5] == 2);
    CHECK(f[-1] == 2);
    CHECK(f[-2] == 1);
    CHECK(f[-3] == 0);
    CHECK(f[-4] == 2);
    CHECK(f[-5] == 1);
    CHECK(f[-6] == 0);

    CHECK(*m == 1);
    CHECK(m[0] == 1);
    CHECK(m[1] == 2);
    CHECK(m[2] == 0);
    CHECK(m[3] == 1);
    CHECK(m[4] == 2);
    CHECK(m[5] == 0);

    CHECK(m[-1] == 0);
    CHECK(m[-2] == 2);
    CHECK(m[-3] == 1);
    CHECK(m[-4] == 0);
    CHECK(m[-5] == 2);
    CHECK(m[-6] == 1);

    CHECK(*l == 2);
    CHECK(l[0] == 2);
    CHECK(l[1] == 0);
    CHECK(l[2] == 1);
    CHECK(l[3] == 2);
    CHECK(l[4] == 0);
    CHECK(l[5] == 1);

    CHECK(l[-1] == 1);
    CHECK(l[-2] == 0);
    CHECK(l[-3] == 2);
    CHECK(l[-4] == 1);
    CHECK(l[-5] == 0);
    CHECK(l[-6] == 2);

    CHECK(f != e);
}

int main()
{
    // initializer list
    {
        auto il = {0, 1, 2};
        test_random_access_it(il);
        test_const_forward_reversed_range(il);

        const auto cil = {0, 1, 2};
        test_random_access_it(cil);
        test_const_forward_reversed_range(cil);
    }

    // array
    {
        std::array<int, 3> a = {{0, 1, 2}};
        test_random_access_it(a);
        test_mutable_forward_range_reversed(a);

        const std::array<int, 3> ca = {{0, 1, 2}};
        test_random_access_it(ca);
        test_const_forward_reversed_range(ca);
    }

    // list
    {
        std::list<int> l = {0, 1, 2};
        test_bidirectional_it(l);
        test_mutable_forward_range_reversed(l);

        const std::list<int> cl = {0, 1, 2};
        test_bidirectional_it(cl);
        test_const_forward_reversed_range(cl);
    }

    // forward list
    {
        std::forward_list<int> l = {0, 1, 2};
        test_forward_it(l);
        test_const_forward_range(l);

        const std::forward_list<int> cl = {0, 1, 2};
        test_forward_it(cl);
        test_const_forward_range(cl);
    }

    // move-only types
    {
        std::array<std::unique_ptr<int>, 3> a = {{
            std::unique_ptr<int>(new int(0)),
            std::unique_ptr<int>(new int(1)),
            std::unique_ptr<int>(new int(2))
        }};
        auto r = a | view::cycle;
        auto b = iter_move(r.begin() + 4);
        CHECK((*(b)) == 1);
    }

    // infinite
    {
        int count = 0;
        auto il = {0, 1, 2};
        auto v = 10;
        RANGES_FOR(auto&& i, il | view::cycle)
        {
            if (count == 42) { break; }
            v = i;
            ++count;
        }
        CHECK(count == 42);
        CHECK(v == 2);
    }

    // non-bounded
    {
        auto sz = view::c_str((char const *)"hi! ");
        ::check_equal(
            sz | view::cycle | view::take(10),
            {'h','i','!',' ','h','i','!',' ','h','i'} );

        auto rng = sz | view::cycle;
        auto it = ranges::begin(rng);
        CHECK(*it == 'h');
        CHECK(*++it == 'i');
        CHECK(*++it == '!');
        CHECK(*++it == ' ');
        CHECK(*++it == 'h');
        CHECK(*--it == ' ');
        CHECK(*--it == '!');
        CHECK(*--it == 'i');
        CHECK(*--it == 'h');

        rng = sz | view::cycle;
        it = ranges::begin(rng);
        it += 4;
        CHECK(*it == 'h');
    }

    return test_result();
}
