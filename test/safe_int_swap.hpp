/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2013-2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGES_V3_TEST_SAFE_INT_SWAP_HPP
#define RANGES_V3_TEST_SAFE_INT_SWAP_HPP

#include <range/v3/utility/safe_int.hpp>

namespace ranges
{

RANGES_CXX14_CONSTEXPR
void swap(safe_int<int>& a, safe_int<int>& b)
{
    safe_int<int> tmp = std::move(a);
    a = std::move(b);
    b = std::move(tmp);
}

}  // namespace ranges

#endif
