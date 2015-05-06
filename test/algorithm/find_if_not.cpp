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

//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include <utility>
#include <range/v3/core.hpp>
#include <range/v3/algorithm/find_if_not.hpp>
#include "../simple_test.hpp"
#include "../test_iterators.hpp"

#ifdef RANGES_CXX_GREATER_THAN_11
constexpr bool is_three(int i) { return i == 3; }

template<class Rng>
RANGES_CXX14_CONSTEXPR bool contains_other_than_three(Rng r) {
    auto it = ranges::find_if_not(r, is_three);
    return it != ranges::end(r);
}
#endif

int main()
{
    using namespace ranges;


#ifdef RANGES_CXX_GREATER_THAN_11
    {
        static_assert(contains_other_than_three(std::initializer_list<int>{3, 3, 2, 3}), "");
        static_assert(!contains_other_than_three(std::initializer_list<int>{3, 3, 3}), "");
    }
#endif

    return ::test_result();
}
