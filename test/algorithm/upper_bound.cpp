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
//
//  Copyright 2005 - 2007 Adobe Systems Incorporated
//  Distributed under the MIT License(see accompanying file LICENSE_1_0_0.txt
//  or a copy at http://stlab.adobe.com/licenses.html)

#include <utility>
#include <range/v3/core.hpp>
#include <range/v3/algorithm/upper_bound.hpp>
#include "../simple_test.hpp"

int main()
{
    using ranges::begin;
    using ranges::end;
    using ranges::size;
    using ranges::less;

    std::pair<int, int> a[] = {{0, 0}, {0, 1}, {1, 2}, {1, 3}, {3, 4}, {3, 5}};
    const std::pair<int, int> c[] = {{0, 0}, {0, 1}, {1, 2}, {1, 3}, {3, 4}, {3, 5}};

    CHECK(ranges::aux::upper_bound_n(begin(a), size(a), a[0]) == &a[1]);
    CHECK(ranges::aux::upper_bound_n(begin(a), size(a), a[1], less()) == &a[2]);
    CHECK(ranges::aux::upper_bound_n(begin(a), size(a), 1, less(), &std::pair<int, int>::first) == &a[4]);

    CHECK(ranges::upper_bound(begin(a), end(a), a[0]) == &a[1]);
    CHECK(ranges::upper_bound(begin(a), end(a), a[1], less()) == &a[2]);
    CHECK(ranges::upper_bound(begin(a), end(a), 1, less(), &std::pair<int, int>::first) == &a[4]);

    CHECK(ranges::upper_bound(a, a[2]) == &a[3]);
    CHECK(ranges::upper_bound(c, c[3]) == &c[4]);

    CHECK(ranges::upper_bound(a, a[4], less()) == &a[5]);
    CHECK(ranges::upper_bound(c, c[5], less()) == &c[6]);

    CHECK(ranges::upper_bound(a, 1, less(), &std::pair<int, int>::first) == &a[4]);
    CHECK(ranges::upper_bound(c, 1, less(), &std::pair<int, int>::first) == &c[4]);

    CHECK(ranges::upper_bound(ranges::view::all(a), a[2]).get_unsafe() == &a[3]);
    CHECK(ranges::upper_bound(ranges::view::all(c), c[3]).get_unsafe() == &c[4]);

    CHECK(ranges::upper_bound(ranges::view::all(a), a[4], less()).get_unsafe() == &a[5]);
    CHECK(ranges::upper_bound(ranges::view::all(c), c[5], less()).get_unsafe() == &c[6]);

    CHECK(ranges::upper_bound(ranges::view::all(a), 1, less(), &std::pair<int, int>::first).get_unsafe() == &a[4]);
    CHECK(ranges::upper_bound(ranges::view::all(c), 1, less(), &std::pair<int, int>::first).get_unsafe() == &c[4]);

#ifdef RANGES_CXX_GREATER_THAN_11
    {
        using namespace ranges;
        constexpr std::pair<int, int> a[] = {{0, 0}, {0, 1}, {1, 2}, {1, 3}, {3, 4}, {3, 5}};

        static_assert(aux::upper_bound_n(begin(a), size(a), a[0]) == &a[1], "");
        static_assert(aux::upper_bound_n(begin(a), size(a), a[1], less()) == &a[2], "");

        static_assert(upper_bound(begin(a), end(a), a[0]) == &a[1], "");
        static_assert(upper_bound(begin(a), end(a), a[1], less()) == &a[2], "");
        static_assert(upper_bound(a, a[2]) == &a[3], "");
        static_assert(upper_bound(a, a[3], less()) == &a[4], "");

        static_assert(upper_bound(a, std::make_pair(1, 3), less()) == &a[4], "");
        // TODO: constexpr
        // requires view::all to be constexpr
        // static_assert(upper_bound(view::all(a), std::make_pair(1, 3), less()).get_unsafe() == &a[4], "");
    }
#endif


    return test_result();
}
