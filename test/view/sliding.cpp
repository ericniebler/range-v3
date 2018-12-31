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
#include <range/v3/view/zip.hpp>
#include <range/v3/view/group_by.hpp>
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

    template<typename Adapted>
    void test_bounded(Adapted& a, std::true_type)
    {
        ::models<concepts::BoundedRange>(a);
    }
    // template<typename Adapted>
    // void test_bounded(Adapted&, std::false_type)
    // {}

    template<typename Adapted>
    void test_prev(Adapted& a, iterator_t<Adapted> const& it, std::true_type)
    {
        ::models<concepts::BidirectionalRange>(a);
        ::check_equal(*ranges::prev(it, 3), view::iota(N - K - 2, N - 2));
    }
    template<typename Adapted>
    void test_prev(Adapted&, iterator_t<Adapted> const&, std::false_type)
    {}

    template<typename Rng,
        bool = RandomAccessRange<Rng>() || (BidirectionalRange<Rng>() && BoundedRange<Rng>())>
    struct size_compare
    {
        iterator_t<Rng> iter1_;
        iterator_t<Rng> iter2_;
        range_difference_type_t<Rng> dist_;
    };
    template<typename Rng>
    struct size_compare<Rng, true>
    {
        iterator_t<Rng> iter1_;
        range_difference_type_t<Rng> dist_;
    };

    template<typename Base>
    void test_finite()
    {
        Base v = view::iota(0,N);
        auto rng = v | view::sliding(K);
        using Adapted = decltype(rng);
        test_size(rng, SizedRange<Base>());

        CONCEPT_ASSERT(Same<
            meta::_t<iterator_concept<iterator_t<Base>>>,
            meta::_t<iterator_concept<iterator_t<Adapted>>>>());

        auto it = ranges::begin(rng);
        test_bounded(rng, BoundedRange<Base>());

        for (auto i = 0; i <= N - K; ++i)
        {
            ::check_equal(*it++, view::iota(i, i + K));
        }
        CHECK(it == ranges::end(rng));

        test_prev(rng, it, BidirectionalRange<Base>());
        CHECK(sizeof(it) == sizeof(size_compare<Base>));
    }
}

void bug_975()
{
    std::vector<double> v{2.0, 2.0, 3.0, 1.0};
    std::vector<int> i{1, 2, 1, 2};
    std::vector<int> t{1, 1, 2, 2};
    {
        using namespace ranges;
        auto vals = view::zip(v, i, t);
        using T = std::tuple<double, int, int>;
        auto g = vals | view::group_by(
            [](T t1, T t2)
            {
                return std::get<2>(t1) == std::get<2>(t2);
            }
        );

        auto windows = view::sliding(g, 2);
        auto it = std::begin(windows);
        (void)it;
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
        auto rng = view::repeat(5) | view::sliding(K);
        ::models<concepts::RandomAccessRange>(rng);
        auto it = rng.begin();
        CONCEPT_ASSERT(RandomAccessIterator<decltype(it)>());
#if defined(__GNUC__) && !defined(__clang__) && \
    ((__GNUC__ == 6 && __GNUC_MINOR__ < 3) || __GNUC__ < 6)
        // Avoid https://gcc.gnu.org/bugzilla/show_bug.cgi?id=78047
        {
            auto deref = *it;
            auto i = deref.begin();
            for (auto n = 0; n < K; ++n) {
                RANGES_ENSURE(i != deref.end());
                CHECK(*i == 5);
                ++i;
            }
            CHECK(i == deref.end());
        }
        auto it2 = next(it, 42);
        CHECK(it != it2);
        {
            auto deref = *it;
            auto i = deref.begin();
            for (auto n = 0; n < K; ++n) {
                RANGES_ENSURE(i != deref.end());
                CHECK(*i == 5);
                ++i;
            }
            CHECK(i == deref.end());
        }
#else
        ::check_equal(*it, view::repeat_n(5, K));
        auto it2 = next(it, 42);
        CHECK(it != it2);
        ::check_equal(*it2, view::repeat_n(5, K));
#endif
    }

    {
        // An infinite, cyclic range with cycle length == K
        auto rng = view::iota(0, K) | view::cycle | view::sliding(K);
        ::models<concepts::RandomAccessRange>(rng);
        auto it = rng.begin();
        CONCEPT_ASSERT(RandomAccessIterator<decltype(it)>());
        for (auto i = 0; i < 42; ++i)
        {
            ::check_equal(*it++, {0,1,2});
            ::check_equal(*it++, {1,2,0});
            ::check_equal(*it++, {2,0,1});
        }
        auto it2 = next(it, 42 * K);
        ::check_equal(*it2, {0,1,2});
    }

    {
        // An infinite, cyclic range with cycle length > K
        auto rng = view::iota(0,7) | view::cycle | view::sliding(K);
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
