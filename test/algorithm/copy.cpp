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
#include <utility>
#include <algorithm>
#include <range/v3/core.hpp>
#include <range/v3/algorithm/copy.hpp>
#include <range/v3/algorithm/equal.hpp>
#include <range/v3/view/delimit.hpp>
#include "../simple_test.hpp"

#if RANGES_CXX_CONSTEXPR >= RANGES_CXX_CONSTEXPR_14
RANGES_CXX14_CONSTEXPR
bool test_constexpr_copy()
{
    int a[4] = {0, 0, 0, 0};
    int const b[4] = {1, 2, 3, 4};
    ranges::copy(b, a);
    return ranges::equal(b, a);
}

static_assert(test_constexpr_copy(), "");
#endif

int main()
{
    using ranges::begin;
    using ranges::end;
    using ranges::size;

    std::pair<int, int> const a[] = {{0, 0}, {0, 1}, {1, 2}, {1, 3}, {3, 4}, {3, 5}};
    static_assert(size(a) == 6, "");
    std::pair<int, int> out[size(a)] = {};

    auto res = ranges::copy(begin(a), end(a), out);
    CHECK(res.first == end(a));
    CHECK(res.second == out + size(out));
    CHECK(&res.first == &res.in());
    CHECK(&res.second == &res.out());
    CHECK(std::equal(a, a + size(a), out));

    std::fill_n(out, size(out), std::make_pair(0, 0));
    CHECK(!std::equal(a, a + size(a), out));

    res = ranges::copy(a, out);
    CHECK(res.first == a + size(a));
    CHECK(res.second == out + size(out));
    CHECK(std::equal(a, a + size(a), out));

    std::fill_n(out, size(out), std::make_pair(0, 0));

    using ranges::view::delimit;
    {
        char const *sz = "hello world";
        char buf[50];
        auto str = delimit(sz, '\0');
        auto res3 = ranges::copy(str, buf);
        *res3.second = '\0';
        CHECK(res3.first == std::next(begin(str), static_cast<std::ptrdiff_t>(std::strlen(sz))));
        CHECK(res3.second == buf + std::strlen(sz));
        CHECK(std::strcmp(sz, buf) == 0);
    }

    {
        char const *sz = "hello world";
        char buf[50];
        auto str = delimit(sz, '\0');
        auto res3 = ranges::copy(std::move(str), buf);
        *res3.second = '\0';
        CHECK(res3.first.get_unsafe() == std::next(begin(str), static_cast<std::ptrdiff_t>(std::strlen(sz))));
        CHECK(res3.second == buf + std::strlen(sz));
        CHECK(std::strcmp(sz, buf) == 0);
    }

    return test_result();
}
