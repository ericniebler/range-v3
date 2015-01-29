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

#include <cstring>
#include <utility>
#include <algorithm>
#include <range/v3/core.hpp>
#include <range/v3/algorithm/copy_backward.hpp>
#include <range/v3/view/delimit.hpp>
#include "../simple_test.hpp"

int main()
{
    using ranges::begin;
    using ranges::end;
    using ranges::size;

    std::pair<int, int> const a[] = {{0, 0}, {0, 1}, {1, 2}, {1, 3}, {3, 4}, {3, 5}};
    static_assert(size(a) == 6, "");
    std::pair<int, int> out[size(a)] = {};

    auto res = ranges::copy_backward(begin(a), end(a), end(out));
    CHECK(res.first == end(a));
    CHECK(res.second == begin(out));
    CHECK(std::equal(a, a + size(a), out));

    std::fill_n(out, size(out), std::make_pair(0, 0));
    CHECK(!std::equal(a, a + size(a), out));

    res = ranges::copy_backward(a, end(out));
    CHECK(res.first == end(a));
    CHECK(res.second == begin(out));
    CHECK(std::equal(a, a + size(a), out));

    return test_result();
}
