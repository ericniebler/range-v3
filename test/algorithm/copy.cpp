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
#include <array>
#include <range/v3/core.hpp>
#include <range/v3/algorithm/copy.hpp>
#include <range/v3/algorithm/equal.hpp>
#include <range/v3/view/delimit.hpp>
#include <range/v3/utility/array.hpp>
#include "../simple_test.hpp"

#ifdef RANGES_CXX_GREATER_THAN_11
template<typename Rng>
RANGES_RELAXED_CONSTEXPR auto copy_rng(Rng&& input)  {
    ranges::array<int, 4> tmp{{0, 0, 0, 0}};
    auto res = ranges::copy(input, ranges::begin(tmp));
    if (res.first != ranges::end(input)) { throw 0; };
    if (res.second != ranges::end(tmp)) { throw 0; };
    return tmp;
}
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
        CHECK(res3.first == std::next(begin(str), std::strlen(sz)));
        CHECK(res3.second == buf + std::strlen(sz));
        CHECK(std::strcmp(sz, buf) == 0);
    }

    {
        char const *sz = "hello world";
        char buf[50];
        auto str = delimit(sz, '\0');
        auto res3 = ranges::copy(std::move(str), buf);
        *res3.second = '\0';
        CHECK(res3.first.get_unsafe() == std::next(begin(str), std::strlen(sz)));
        CHECK(res3.second == buf + std::strlen(sz));
        CHECK(std::strcmp(sz, buf) == 0);
    }

#ifdef RANGES_CXX_GREATER_THAN_11
    {
        constexpr auto a1 = copy_rng(ranges::array<int,4>{{0, 1, 2, 3}});
        static_assert(ranges::equal(a1, {0, 1, 2, 3}), "");
    }
#endif

    return test_result();
}
