//  Copyright Eric Niebler 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/

#include <range/v3/range.hpp>
#include "../simple_test.hpp"

int main()
{
    int v1[] = { 0, 2, 2, 4, 6 };
    CHECK(ranges::adjacent_find(ranges::begin(v1), ranges::end(v1)) == &v1[1]);
    CHECK(ranges::adjacent_find(v1) == &v1[1]);
    static_assert(std::is_same<int const*, decltype(ranges::adjacent_find({0, 2, 2, 4, 6}))>::value, "");

    std::pair<int, int> v2[] = {{0, 0}, {0, 2}, {0, 2}, {0, 4}, {0, 6}};
    CHECK(ranges::adjacent_find(ranges::begin(v2), ranges::end(v2),
        ranges::equal_to{}, &std::pair<int, int>::second) == &v2[1]);
    CHECK(ranges::adjacent_find(v2, ranges::equal_to{}, &std::pair<int, int>::second) == &v2[1]);
    static_assert(std::is_same<std::pair<int,int>*,
                               decltype(ranges::adjacent_find(v2, ranges::equal_to{},
                                    &std::pair<int, int>::second))>::value, "");
    return test_result();
}
