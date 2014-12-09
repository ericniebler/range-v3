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

#include <range/v3/core.hpp>
#include <range/v3/view/iota.hpp>
#include <range/v3/view/take.hpp>
#include <range/v3/view/any_range.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"

int main()
{
    using namespace ranges;

    any_input_range<int, true> ints = view::ints;
    ::check_equal(ints | view::take(10), {0,1,2,3,4,5,6,7,8,9});

    any_input_range<int> ints2 = view::ints | view::take(10);
    ::check_equal(ints2, {0,1,2,3,4,5,6,7,8,9});

    return test_result();
}
