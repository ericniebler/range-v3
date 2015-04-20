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
#include <range/v3/algorithm/merge_move.hpp>
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
    auto r = merge_move(ia, ib, begin(ic));
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

#ifdef RANGES_CXX_GREATER_THAN_11
    {
        static_assert(constexpr_test(), "");
    }
#endif

    return ::test_result();
}
