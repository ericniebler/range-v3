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
#include "../array.hpp"
#include "../simple_test.hpp"

#ifdef RANGES_CXX_GREATER_THAN_11
template<typename Rng>
RANGES_CXX14_CONSTEXPR auto copy_rng(Rng&& input)  {
    array<int, 4> tmp{{0, 0, 0, 0}};
    auto res = ranges::copy_backward(input, ranges::end(tmp));
    if (res.first != ranges::end(input)) { throw 0; };
    if (res.second != ranges::begin(tmp)) { throw 0; };
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

    std::fill_n(out, size(out), std::make_pair(0, 0));
    auto res2 = ranges::copy_backward(ranges::view::all(a), end(out));
    CHECK(res2.first.get_unsafe() == end(a));
    CHECK(res2.second == begin(out));
    CHECK(std::equal(a, a + size(a), out));

#ifdef RANGES_CXX_GREATER_THAN_11
    {
        constexpr auto a1 = copy_rng(array<int,4>{{0, 1, 2, 3}});
        static_assert(ranges::equal(a1, {0, 1, 2, 3}), "");
    }
#endif
    return test_result();
}
