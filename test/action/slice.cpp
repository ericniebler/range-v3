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
#include <range/v3/view/stride.hpp>
#include <range/v3/algorithm/copy.hpp>
#include <range/v3/algorithm/move.hpp>
#include <range/v3/algorithm/equal.hpp>
#include <range/v3/action/slice.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"

int main()
{
    using namespace ranges;

    std::vector<int> v = view::ints(0,100);

    auto v2 = v | copy | action::slice(10,20);
    CHECK(size(v2) == 10u);
    ::models<concepts::Same>(v, v2);
    ::check_equal(v2, {10,11,12,13,14,15,16,17,18,19});

    v2 = v2 | move | action::slice(2,8);
    ::check_equal(v2, {12,13,14,15,16,17});

    v2 |= action::slice(0,0);
    CHECK(v2.size() == 0u);

    auto & v3 = action::slice(v, 90, 100);
    CHECK(&v3 == &v);
    ::check_equal(v, {90,91,92,93,94,95,96,97,98,99});

    return ::test_result();
}
