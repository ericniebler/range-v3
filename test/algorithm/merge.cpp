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
//
//  Copyright 2005 - 2007 Adobe Systems Incorporated
//  Distributed under the MIT License(see accompanying file LICENSE_1_0_0.txt
//  or a copy at http://stlab.adobe.com/licenses.html)

#include <memory>
#include <utility>
#include <algorithm>
#include <range/v3/core.hpp>
#include <range/v3/algorithm/merge.hpp>
#include "../simple_test.hpp"
#include "../test_iterators.hpp"

RANGES_DIAGNOSTIC_IGNORE_SIGN_CONVERSION

int main()
{
    {
        int N = 100000;
        std::unique_ptr<int[]> ia{new int[N]};
        std::unique_ptr<int[]> ib{new int[N]};
        std::unique_ptr<int[]> ic{new int[2 * N]};
        for(int i = 0; i < N; ++i)
            ia[i] = 2 * i;
        for(int i = 0; i < N; ++i)
            ib[i] = 2 * i + 1;
        auto r = ranges::merge(ia.get(), ia.get() + N,
            ib.get(), ib.get() + N, ic.get());
        CHECK(r.in1 == ia.get() + N);
        CHECK(r.in2 == ib.get() + N);
        CHECK(r.out == ic.get() + 2 * N);
        CHECK(ic[0] == 0);
        CHECK(ic[2 * N - 1] == 2 * N - 1);
        CHECK(std::is_sorted(ic.get(), ic.get() + 2 * N));
    }

    {
        int N = 100000;
        std::unique_ptr<int[]> ia{new int[N]};
        std::unique_ptr<int[]> ib{new int[N]};
        std::unique_ptr<int[]> ic{new int[2 * N]};
        for(int i = 0; i < N; ++i)
            ia[i] = 2 * i;
        for(int i = 0; i < N; ++i)
            ib[i] = 2 * i + 1;
        auto r0 = ranges::make_subrange(ia.get(), ia.get() + N);
        auto r1 = ranges::make_subrange(ib.get(), ib.get() + N);
        auto r = ranges::merge(r0, r1, ic.get());
        CHECK(r.in1 == ia.get() + N);
        CHECK(r.in2 == ib.get() + N);
        CHECK(r.out == ic.get() + 2 * N);
        CHECK(ic[0] == 0);
        CHECK(ic[2 * N - 1] == 2 * N - 1);
        CHECK(std::is_sorted(ic.get(), ic.get() + 2 * N));
    }


    {
        int N = 100000;
        std::unique_ptr<int[]> ia{new int[N]};
        std::unique_ptr<int[]> ib{new int[N]};
        std::unique_ptr<int[]> ic{new int[2 * N]};
        for(int i = 0; i < N; ++i)
            ia[i] = 2 * i;
        for(int i = 0; i < N; ++i)
            ib[i] = 2 * i + 1;
        auto r0 = ::MakeTestRange(ia.get(), ia.get() + N);
        auto r1 = ::MakeTestRange(ib.get(), ib.get() + N);
        auto r = ranges::merge(std::move(r0), std::move(r1), ic.get());
        CHECK(::is_dangling(r.in1));
        CHECK(::is_dangling(r.in2));
        CHECK(r.out == ic.get() + 2 * N);
        CHECK(ic[0] == 0);
        CHECK(ic[2 * N - 1] == 2 * N - 1);
        CHECK(std::is_sorted(ic.get(), ic.get() + 2 * N));

        static_assert(std::is_same<decltype(r),
            ranges::merge_result<ranges::dangling, ranges::dangling, int *>>::value, "");
    }

    return ::test_result();
}
