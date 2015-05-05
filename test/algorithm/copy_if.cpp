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

#include <range/v3/algorithm/copy_if.hpp>
#include "../array.hpp"
#include "../simple_test.hpp"

#ifdef RANGES_CXX_GREATER_THAN_11
constexpr bool is_even(int i) { return i % 2 == 0; }
RANGES_CXX14_CONSTEXPR bool test_constexpr()  {
    using namespace ranges;
    array<int, 4> a{{1, 2, 3, 4}};
    array<int, 4> b{{0, 0, 0, 0}};
    auto res = copy_if(a, ranges::begin(b), is_even);
    if (res.first != end(a)) { return false;  };
    if (res.second != begin(b) + 2) { return false; };
    if (a[0] != 1) { return false; }
    if (a[1] != 2) { return false; }
    if (a[2] != 3) { return false; }
    if (a[3] != 4) { return false; }
    if (b[0] != 2) { return false; }
    if (b[1] != 4) { return false; }
    if (b[2] != 0) { return false; }
    if (b[3] != 0) { return false; }
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

    return test_result();
}
