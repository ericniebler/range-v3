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

#include <list>
#include <map>
#include <vector>
#include <range/v3/core.hpp>
#include <range/v3/range/conversion.hpp>
#include <range/v3/view/iota.hpp>
#include <range/v3/view/indices.hpp>
#include <range/v3/view/transform.hpp>
#include <range/v3/view/take.hpp>
#include <range/v3/view/zip.hpp>
#include <range/v3/action/sort.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"

template<typename T>
struct vector_like : std::vector<T>
{
    using std::vector<T>::vector;

    using typename std::vector<T>::size_type;

    size_type last_reservation{};
    size_type reservation_count{};

    void reserve(size_type n)
    {
        std::vector<T>::reserve(n);
        last_reservation = n;
        ++reservation_count;
    }
};

#if RANGES_CXX_DEDUCTION_GUIDES >= RANGES_CXX_DEDUCTION_GUIDES_17
template<
    typename Rng,
    typename CI = ranges::range_common_iterator_t<Rng>,
    typename = decltype(std::map{CI{}, CI{}})>
void test_zip_to_map(Rng &&rng, int)
{
    using namespace ranges;
#ifdef RANGES_WORKAROUND_MSVC_779708
    auto m = static_cast<Rng &&>(rng) | to<std::map>();
#else // ^^^ workaround / no workaround vvv
    auto m = static_cast<Rng &&>(rng) | to<std::map>;
#endif // RANGES_WORKAROUND_MSVC_779708
    CPP_assert(Same<decltype(m), std::map<int, int>>);
}
#endif
template<typename Rng>
void test_zip_to_map(Rng &&, long)
{}

int main()
{
    using namespace ranges;

    {
        auto lst0 = view::ints | view::transform([](int i){return i*i;}) | view::take(10)
            | to<std::list>();
        CPP_assert(Same<decltype(lst0), std::list<int>>);
        ::check_equal(lst0, {0,1,4,9,16,25,36,49,64,81});
    }

#ifndef RANGES_WORKAROUND_MSVC_779708 // "workaround" is a misnomer; there's no workaround.
    {
        auto lst1 = view::ints | view::transform([](int i){return i*i;}) | view::take(10)
            | to<std::list>;
        CPP_assert(Same<decltype(lst1), std::list<int>>);
        ::check_equal(lst1, {0,1,4,9,16,25,36,49,64,81});
    }
#endif // RANGES_WORKAROUND_MSVC_779708

    {
        auto vec0 = view::ints | view::transform([](int i){return i*i;}) | view::take(10)
            | to_vector | action::sort(std::greater<int>{});
        CPP_assert(Same<decltype(vec0), std::vector<int>>);
        ::check_equal(vec0, {81,64,49,36,25,16,9,4,1,0});
    }

    {
        auto vec1 = view::ints | view::transform([](int i){return i*i;}) | view::take(10)
            | to<std::vector<long>>() | action::sort(std::greater<long>{});
        CPP_assert(Same<decltype(vec1), std::vector<long>>);
        ::check_equal(vec1, {81,64,49,36,25,16,9,4,1,0});
    }

#ifndef RANGES_WORKAROUND_MSVC_779708
    {
        auto vec2 = view::ints | view::transform([](int i){return i*i;}) | view::take(10)
            | to<std::vector<long>> | action::sort(std::greater<long>{});
        CPP_assert(Same<decltype(vec2), std::vector<long>>);
        ::check_equal(vec2, {81,64,49,36,25,16,9,4,1,0});
    }
#endif // RANGES_WORKAROUND_MSVC_779708

    {
        const std::size_t N = 4096;
        auto vl = view::iota(0, int{N}) | to<vector_like<int>>();
        CPP_assert(Same<decltype(vl), vector_like<int>>);
        CHECK(vl.reservation_count == std::size_t{1});
        CHECK(vl.last_reservation == N);
    }

    // https://github.com/ericniebler/range-v3/issues/1145
    {
        auto r1 = view::indices( std::uintmax_t{ 100 } );
        auto r2 = view::zip( r1, r1 );

        std::map<std::uintmax_t, std::uintmax_t> m = r2;
        (void) m;
        (void)(r2 | ranges::to<std::map<std::uintmax_t, std::uintmax_t>>);
    }

    test_zip_to_map(view::zip(view::ints, view::iota(0, 10)), 0);

    return ::test_result();
}
