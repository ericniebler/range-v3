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
//
//  Copyright 2005 - 2007 Adobe Systems Incorporated
//  Distributed under the MIT License(see accompanying file LICENSE_1_0_0.txt
//  or a copy at http://stlab.adobe.com/licenses.html)

#include <memory>
#include <utility>
#include <algorithm>
#include <range/v3/core.hpp>
#include <range/v3/algorithm/merge.hpp>
#include <range/v3/algorithm/is_sorted.hpp>
#include <range/v3/utility/array.hpp>
#include "../simple_test.hpp"

#ifdef RANGES_CXX_GREATER_THAN_11
RANGES_RELAXED_CONSTEXPR bool constexpr_test() {
    using namespace ranges;
    constexpr unsigned N = 100;
    array<int, N> ia{{0}};
    array<int, N> ib{{0}};
    array<int, 2*N> ic{{0}};
    for(unsigned i = 0; i < N; ++i)
        ia[i] = 2 * i;
    for(unsigned i = 0; i < N; ++i)
        ib[i] = 2 * i + 1;
    auto r = merge(ia, ib, begin(ic));
    if(std::get<0>(r) != end(ia)) { return false; }
    if(std::get<1>(r) != end(ib)) { return false; }
    if(std::get<2>(r) != end(ic)) { return false; }
    if(ic[0] != 0) { return false; }
    if(ic[2 * N - 1] != (int)(2 * N - 1)) { return false; }
    if(!is_sorted(ic)) { return false; }
    return true;
}
#endif

int main()
{
    {
        unsigned N = 100000;
        std::unique_ptr<int[]> ia{new int[N]};
        std::unique_ptr<int[]> ib{new int[N]};
        std::unique_ptr<int[]> ic{new int[2 * N]};
        for(unsigned i = 0; i < N; ++i)
            ia[i] = 2 * i;
        for(unsigned i = 0; i < N; ++i)
            ib[i] = 2 * i + 1;
        auto r = ranges::merge(ia.get(), ia.get() + N,
            ib.get(), ib.get() + N, ic.get());
        CHECK(std::get<0>(r) == ia.get() + N);
        CHECK(std::get<1>(r) == ib.get() + N);
        CHECK(std::get<2>(r) == ic.get() + 2 * N);
        CHECK(ic[0] == 0);
        CHECK(ic[2 * N - 1] == (int)(2 * N - 1));
        CHECK(std::is_sorted(ic.get(), ic.get() + 2 * N));
    }

    {
        unsigned N = 100000;
        std::unique_ptr<int[]> ia{new int[N]};
        std::unique_ptr<int[]> ib{new int[N]};
        std::unique_ptr<int[]> ic{new int[2 * N]};
        for(unsigned i = 0; i < N; ++i)
            ia[i] = 2 * i;
        for(unsigned i = 0; i < N; ++i)
            ib[i] = 2 * i + 1;
        auto r0 = ranges::make_range(ia.get(), ia.get() + N);
        auto r1 = ranges::make_range(ib.get(), ib.get() + N);
        auto r = ranges::merge(r0, r1, ic.get());
        CHECK(std::get<0>(r) == ia.get() + N);
        CHECK(std::get<1>(r) == ib.get() + N);
        CHECK(std::get<2>(r) == ic.get() + 2 * N);
        CHECK(ic[0] == 0);
        CHECK(ic[2 * N - 1] == (int)(2 * N - 1));
        CHECK(std::is_sorted(ic.get(), ic.get() + 2 * N));
    }


    {
        unsigned N = 100000;
        std::unique_ptr<int[]> ia{new int[N]};
        std::unique_ptr<int[]> ib{new int[N]};
        std::unique_ptr<int[]> ic{new int[2 * N]};
        for(unsigned i = 0; i < N; ++i)
            ia[i] = 2 * i;
        for(unsigned i = 0; i < N; ++i)
            ib[i] = 2 * i + 1;
        auto r0 = ranges::make_range(ia.get(), ia.get() + N);
        auto r1 = ranges::make_range(ib.get(), ib.get() + N);
        auto r = ranges::merge(std::move(r0), std::move(r1), ic.get());
        CHECK(std::get<0>(r).get_unsafe() == ia.get() + N);
        CHECK(std::get<1>(r).get_unsafe() == ib.get() + N);
        CHECK(std::get<2>(r) == ic.get() + 2 * N);
        CHECK(ic[0] == 0);
        CHECK(ic[2 * N - 1] == (int)(2 * N - 1));
        CHECK(std::is_sorted(ic.get(), ic.get() + 2 * N));

        // Some tests for sanitizing an algorithm result
        static_assert(std::is_same<decltype(r), std::tuple<ranges::dangling<int *>, ranges::dangling<int *>, int *>>::value, "");
        auto r2 = ranges::sanitize(r);
        static_assert(std::is_same<decltype(r2), std::tuple<ranges::dangling<>, ranges::dangling<>, int *>>::value, "");
        auto r3 = ranges::sanitize(const_cast<decltype(r) const &>(r));
        static_assert(std::is_same<decltype(r3), std::tuple<ranges::dangling<>, ranges::dangling<>, int *>>::value, "");
        auto r4 = ranges::sanitize(std::move(r));
        static_assert(std::is_same<decltype(r4), std::tuple<ranges::dangling<>, ranges::dangling<>, int *>>::value, "");
    }

#ifdef RANGES_CXX_GREATER_THAN_11
    {
        static_assert(constexpr_test(), "");
    }
#endif

    return ::test_result();
}
