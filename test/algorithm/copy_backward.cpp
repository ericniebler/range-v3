// Range v3 library
//
//  Copyright Eric Niebler 2014-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3

#include <cstring>
#include <algorithm>
#include <utility>
#include <vector>
#include <range/v3/core.hpp>
#include <range/v3/algorithm/copy_backward.hpp>
#include "../simple_test.hpp"
#include "../test_iterators.hpp"

int main()
{
    using ranges::begin;
    using ranges::end;
    using ranges::size;

    std::pair<int, int> const a[] = {{0, 0}, {0, 1}, {1, 2}, {1, 3}, {3, 4}, {3, 5}};
    static_assert(size(a) == 6, "");
    std::pair<int, int> out[size(a)] = {};

    {
        auto res = ranges::copy_backward(begin(a), end(a), end(out));
        CHECK(res.in == end(a));
        CHECK(res.out == begin(out));
        CHECK(std::equal(a, a + size(a), out));
    }

    {
        std::fill_n(out, size(out), std::make_pair(0, 0));
        auto res = ranges::copy_backward(a, end(out));
        CHECK(res.in == end(a));
        CHECK(res.out == begin(out));
        CHECK(std::equal(a, a + size(a), out));
    }

#ifndef RANGES_WORKAROUND_MSVC_573728
    {
        std::fill_n(out, size(out), std::make_pair(0, 0));
        auto res = ranges::copy_backward(std::move(a), end(out));
        CHECK(::is_dangling(res.in));
        CHECK(res.out == begin(out));
        CHECK(std::equal(a, a + size(a), out));
    }
#endif

    {
        std::fill_n(out, size(out), std::make_pair(0, 0));
        std::vector<std::pair<int, int>> vec(begin(a), end(a));
        auto res = ranges::copy_backward(std::move(vec), end(out));
        CHECK(::is_dangling(res.in));
        CHECK(res.out == begin(out));
        CHECK(std::equal(a, a + size(a), out));
    }

    {
        std::fill_n(out, size(out), std::make_pair(0, 0));
        auto res = ranges::copy_backward(ranges::views::all(a), end(out));
        CHECK(res.in == end(a));
        CHECK(res.out == begin(out));
        CHECK(std::equal(a, a + size(a), out));
    }

    return test_result();
}
