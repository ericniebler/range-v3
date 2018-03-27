// Range v3 library
//
//  Copyright Eric Niebler 2014-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <array>
#include <vector>
#include <range/v3/core.hpp>
#include <range/v3/view/iota.hpp>
#include <range/v3/algorithm/copy.hpp>
#include <range/v3/algorithm/move.hpp>
#include <range/v3/algorithm/equal.hpp>
#include <range/v3/action/stride.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"

int main()
{
    using namespace ranges;

    std::vector<int> v = view::ints(0,100);

    auto v2 = v | copy | action::stride(10);
    CHECK(size(v2) == 10u);
    ::models<concepts::Same>(v, v2);
    ::check_equal(v2, {0,10,20,30,40,50,60,70,80,90});

    v2 = v2 | move | action::stride(4);
    ::check_equal(v2, {0,40,80});

    v2 |= action::stride(2);
    ::check_equal(v2, {0,80});
    v2 |= action::stride(1);
    ::check_equal(v2, {0,80});
    v2 |= action::stride(10);
    ::check_equal(v2, {0});

    auto & v3 = action::stride(v, 30);
    CHECK(&v3 == &v);
    ::check_equal(v, {0,30,60,90});

    return ::test_result();
}
