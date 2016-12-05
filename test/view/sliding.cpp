// Range v3 library
//
//  Copyright Tobias Mayer 2016
//  Copyright Casey Carter 2016
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
#include <range/v3/core.hpp>
#include <range/v3/view/cycle.hpp>
#include <range/v3/view/iota.hpp>
#include <range/v3/view/repeat.hpp>
#include <range/v3/view/repeat_n.hpp>
#include <range/v3/view/reverse.hpp>
#include <range/v3/view/sliding.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"

using namespace ranges;

namespace
{
    constexpr auto N = 7;
    constexpr auto K = 3;
    CONCEPT_ASSERT(K < N);

    template<typename Adapted>
    void test_size(Adapted& a, std::true_type)
    {
        ::models<concepts::SizedRange>(a);
        CHECK(size(a), unsigned(N - K + 1));
    }
    template<typename Adapted>
    void test_size(Adapted&, std::false_type)
    {}

    template<typename Base, typename Adapted>
    void test_bounded(Base const& b, Adapted& a, std::true_type)
    {
        ::models<concepts::BoundedRange>(a);
        CHECK(ranges::end(a).base() == ranges::end(b));
    }
    template<typename Base, typename Adapted>
    void test_bounded(Base const&, Adapted&, std::false_type)
    {}

    template<typename Adapted>
    void test_prev(Adapted& a, range_iterator_t<Adapted> const& it, std::true_type)
    {
        ::models<concepts::BidirectionalRange>(a);
        ::check_equal(*ranges::prev(it, 3), view::iota(N - K - 2, N - 2));
    }
    template<typename Adapted>
    void test_prev(Adapted&, range_iterator_t<Adapted> const&, std::false_type)
    {}

    template<typename I, typename D, bool = (bool) RandomAccessIterator<I>()>
    struct size_compare
    {
        I iter1_;
        I iter2_;
        D dist_;
    };
    template<typename I, typename D>
    struct size_compare<I, D, true>
    {
        I iter1_;
        D dist_;
    };

    template<typename Base>
    void test_finite()
    {
        Base v = view::iota(0,N);
        auto rng = v | view::sliding(K);
        using Adapted = decltype(rng);
        test_size(rng, SizedRange<Base>());

        CONCEPT_ASSERT(Same<
            meta::_t<iterator_concept<range_iterator_t<Base>>>,
            meta::_t<iterator_concept<range_iterator_t<Adapted>>>>());

        auto it = ranges::begin(rng);
        CHECK(it.base() == ranges::next(v.begin(), K - 1));
        test_bounded(v, rng, BoundedRange<Base>());

        for (auto i = 0; i <= N - K; ++i) {
            ::check_equal(*it++, view::iota(i, i + K));
        }
        CHECK(it.base() == v.end());
        CHECK(it == ranges::end(rng));

        test_prev(rng, it, BidirectionalRange<Base>());

        if (!ranges::v3::detail::broken_ebo) {
            using SC = size_compare<
                range_iterator_t<Base>, range_difference_t<Base>>;
            CHECK(sizeof(it) == sizeof(SC));
        }
    }
}

int main()
{
    test_finite<std::forward_list<int>>();
    test_finite<std::list<int>>();
    test_finite<std::vector<int>>();
    test_finite<decltype(view::iota(0,N))>();

    {
        // An infinite, cyclic range with cycle length == 1
        auto fives = view::repeat(5);
        ::models<concepts::RandomAccessRange>(fives);
        auto rng = fives | view::sliding(3);
        ::models<concepts::RandomAccessRange>(rng);
        auto it = rng.begin();
        ::check_equal(*it, view::repeat_n(5, K));
        CONCEPT_ASSERT(RandomAccessIterator<decltype(it)>());
        auto it2 = next(it, 42);
        CHECK(it == it2);
        ::check_equal(*it2, view::repeat_n(5, K));
    }

    {
        // An infinite, cyclic range with cycle length == K
        auto cyc = view::iota(0, K) | view::cycle;
        auto rng = cyc | view::sliding(K);
        ::models<concepts::RandomAccessRange>(rng);
        auto it = rng.begin();
        CONCEPT_ASSERT(RandomAccessIterator<decltype(it)>());
        for (auto i = 0; i < 42; ++i) {
            ::check_equal(*it++, {0,1,2});
            ::check_equal(*it++, {1,2,0});
            ::check_equal(*it++, {2,0,1});
        }
        auto it2 = next(it, 42 * K);
        ::check_equal(*it2, {0,1,2});
    }

    {
        // An infinite, cyclic range with cycle length > K
        auto cyc = view::iota(0,7) | view::cycle;
        auto rng = cyc | view::sliding(K);
        //[0,1,2],[1,2,3],[2,3,4],[3,4,5],[4,5,6],[5,6,0],[6,0,1],...
        auto it = rng.begin();
        CONCEPT_ASSERT(RandomAccessIterator<decltype(it)>());
        ::check_equal(*it, {0,1,2});
        ::check_equal(*next(it, 2), {2,3,4});
        ::check_equal(*next(it,16), {2,3,4});
        ::check_equal(*next(it,27), {6,0,1});
    }

    return ::test_result();
}
