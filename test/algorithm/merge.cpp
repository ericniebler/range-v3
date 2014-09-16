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
#include "../simple_test.hpp"

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
        CHECK(std::get<2>(r) == ic.get() + 2 * N);
        CHECK(ic[0] == 0);
        CHECK(ic[2 * N - 1] == (int)(2 * N - 1));
        CHECK(std::is_sorted(ic.get(), ic.get() + 2 * N));
    }

    return ::test_result();
}
