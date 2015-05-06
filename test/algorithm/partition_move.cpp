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

//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include <tuple>
#include <range/v3/core.hpp>
#include <range/v3/algorithm/partition_move.hpp>
#include "../simple_test.hpp"

struct is_odd
{
    constexpr bool operator()(const int& i) const {return i & 1;}
};

#ifdef RANGES_CXX_GREATER_THAN_11
RANGES_CXX14_CONSTEXPR bool test_constexpr()
{
    using namespace ranges;
    const int ia[] = {1, 2, 3, 4, 6, 8, 5, 7};
    int r1[10] = {0};
    int r2[10] = {0};
    typedef std::tuple<int const *, int*,  int*> P;
    P p = partition_move(ia, r1, r2, is_odd());
    if(std::get<0>(p) != std::end(ia)) { return false; }
    if(std::get<1>(p) != r1 + 4) { return false; }
    if(r1[0] != 1) { return false; }
    if(r1[1] != 3) { return false; }
    if(r1[2] != 5) { return false; }
    if(r1[3] != 7) { return false; }
    if(std::get<2>(p) != r2 + 4) { return false; }
    if(r2[0] != 2) { return false; }
    if(r2[1] != 4) { return false; }
    if(r2[2] != 6) { return false; }
    if(r2[3] != 8) { return false; }
    return true;
}
#endif

int main()
{

#ifdef RANGES_CXX_GREATER_THAN_11
    {
        static_assert(test_constexpr(), "");
    }
#endif

    return ::test_result();
}
