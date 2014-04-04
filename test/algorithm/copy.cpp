//  Copyright Eric Niebler 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/

#include <cstring>
#include <utility>
#include <algorithm>
#include <range/v3/core.hpp>
#include <range/v3/algorithm/copy.hpp>
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

    auto res = ranges::copy(begin(a), end(a), out);
    CHECK(res.first == out + size(out));
    CHECK(res.second == end(a));
    CHECK(std::equal(a, a + size(a), out));

    std::fill_n(out, size(out), std::make_pair(0, 0));
    CHECK(!std::equal(a, a + size(a), out));

    res = ranges::copy(a, out);
    CHECK(res.first == out + size(out));
    CHECK(res.second == a + size(a));
    CHECK(std::equal(a, a + size(a), out));

    std::fill_n(out, size(out), std::make_pair(0, 0));

    int out2[size(a)] = {};
    int const expected[] = {0, 0, 1, 1, 3, 3};

    auto res2 = ranges::copy(begin(a), end(a), out2, &std::pair<int,int>::first);
    CHECK(res2.first == out2 + size(out2));
    CHECK(res2.second == end(a));
    CHECK(std::equal(begin(expected), end(expected), out2));

    std::fill_n(out2, out2 + size(out2), 0);
    CHECK(!std::equal(begin(expected), end(expected), out2));

    res2 = ranges::copy(a, out2, &std::pair<int, int>::first);
    CHECK(res2.first == out2 + size(out2));
    CHECK(res2.second == a + size(a));
    CHECK(std::equal(begin(expected), end(expected), out2));

    std::fill_n(out2, size(out2), 0);

    using ranges::view::delimit;
    char const *sz = "hello world";
    char buf[50];
    auto str = delimit(sz, '\0');
    auto res3 = ranges::copy(str, buf);
    *res3.first = '\0';
    CHECK(res3.first == buf + std::strlen(sz));
    CHECK(res3.second == std::next(begin(str), std::strlen(sz)));
    CHECK(std::strcmp(sz, buf) == 0);

    return test_result();
}
