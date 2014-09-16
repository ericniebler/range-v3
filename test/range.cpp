//  Copyright Eric Niebler 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/

#include <vector>
#include <range/v3/core.hpp>
#include <range/v3/utility/unreachable.hpp>
#include "./simple_test.hpp"
#include "./test_utils.hpp"

static_assert(sizeof(ranges::range<int*, ranges::detail::empty>) == sizeof(int*),
    "Expected range to be compressed");

static_assert(sizeof(ranges::sized_range<int*, ranges::detail::empty>) == sizeof(int*) + sizeof(int),
    "Expected sized_range to be compressed");

int main()
{
    std::vector<int> vi{1,2,3,4};

    ranges::range<std::vector<int>::iterator> r0 {vi.begin(), vi.end()};
    ::models<ranges::concepts::SizedRange>(r0);
    CHECK(r0.size() == 4u);
    CHECK(r0.first == vi.begin());
    CHECK(r0.second == vi.end());
    ++r0.first;
    CHECK(r0.size() == 3u);

    std::pair<std::vector<int>::iterator, std::vector<int>::iterator> p0 = r0;
    CHECK(p0.first == vi.begin()+1);
    CHECK(p0.second == vi.end());

    ranges::range<std::vector<int>::iterator, ranges::unreachable> r1 { r0.begin(), {} };
    static_assert(sizeof(r1) == sizeof(vi.begin()), "");
    ::models<ranges::concepts::Range>(r0);
    ::models_not<ranges::concepts::SizedRange>(r1);
    CHECK(r1.first == vi.begin()+1);
    CHECK(r1.second == ranges::unreachable{});
    r1.second = ranges::unreachable{};

    std::pair<std::vector<int>::iterator, ranges::unreachable> p1 = r1;
    CHECK(p1.first == vi.begin()+1);
    CHECK(p1.second == ranges::unreachable{});
    static_assert(sizeof(p1) > sizeof(r1), "");

    ranges::range<std::vector<int>::iterator, ranges::unreachable> r2 { p1 };
    CHECK(r1.first == vi.begin()+1);
    CHECK(r1.second == ranges::unreachable{});

    return ::test_result();
}
