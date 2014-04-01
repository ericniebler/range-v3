//  Copyright Eric Niebler 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//
//  Copyright 2005 - 2007 Adobe Systems Incorporated
//  Distributed under the MIT License(see accompanying file LICENSE_1_0_0.txt
//  or a copy at http://stlab.adobe.com/licenses.html)

#include <utility>
#include <range/v3/range.hpp>
#include "../simple_test.hpp"

int main()
{
    using ranges::begin;
    using ranges::end;
    using ranges::size;
    using ranges::less;

    std::pair<int, int> a[] = {{0, 0}, {0, 1}, {1, 2}, {1, 3}, {3, 4}, {3, 5}};
    const std::pair<int, int> c[] = {{0, 0}, {0, 1}, {1, 2}, {1, 3}, {3, 4}, {3, 5}};

    CHECK(ranges::lower_bound_n(begin(a), size(a), a[0]).first == &a[0]);
    CHECK(ranges::lower_bound_n(begin(a), size(a), a[1], less()).first == &a[1]);
    CHECK(ranges::lower_bound_n(begin(a), size(a), 1, less(), &std::pair<int, int>::first).first == &a[2]);

    CHECK(ranges::lower_bound(begin(a), end(a), a[0]) == &a[0]);
    CHECK(ranges::lower_bound(begin(a), end(a), a[1], less()) == &a[1]);
    CHECK(ranges::lower_bound(begin(a), end(a), 1, less(), &std::pair<int, int>::first) == &a[2]);

    CHECK(ranges::lower_bound(a, a[2]) == &a[2]);
    CHECK(ranges::lower_bound(c, c[3]) == &c[3]);

    CHECK(ranges::lower_bound(a, a[4], less()) == &a[4]);
    CHECK(ranges::lower_bound(c, c[5], less()) == &c[5]);

    CHECK(ranges::lower_bound(a, 1, less(), &std::pair<int, int>::first) == &a[2]);
    CHECK(ranges::lower_bound(c, 1, less(), &std::pair<int, int>::first) == &c[2]);

    return test_result();
}
