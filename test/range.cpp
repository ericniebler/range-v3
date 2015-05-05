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

#include <list>
#include <vector>
#include <range/v3/core.hpp>
#include <range/v3/utility/unreachable.hpp>
#include <range/v3/view/all.hpp>
#include "./simple_test.hpp"
#include "./test_utils.hpp"
#include "./array.hpp"

struct empty
{};

static_assert(sizeof(ranges::range<int*, empty>) == sizeof(int*),
    "Expected range to be compressed");

static_assert(sizeof(ranges::sized_range<int*, empty>) == sizeof(int*) + sizeof(std::size_t),
    "Expected sized_range to be compressed");

template<typename T, typename U = decltype(std::declval<T>().pop_front())>
int test_pop_front(T & t)
{
    return 0;
}

char* test_pop_front(ranges::detail::any)
{
    return nullptr;
}

#ifdef RANGES_CXX_GREATER_THAN_11
RANGES_CXX14_CONSTEXPR bool test_constexpr()
{
    array<int, 4> vi{{1,2,3,4}};
    using it = typename array<int, 4>::iterator;
    ranges::range<it> r0 {vi.begin(), vi.end()};
    ::models<ranges::concepts::SizedRange>(r0);
    if(r0.size() != 4u) { return false; }
    if(r0.first != vi.begin()) { return false; }
    if(r0.second != vi.end()) { return false; }
    ++r0.first;
    if(r0.size() != 3u) { return false; }

    std::pair<it, it> p0 = r0;
    if(p0.first != vi.begin()+1) { return false; }
    if(p0.second != vi.end()) { return false; }

    ranges::range<it, ranges::unreachable> r1 { r0.begin(), {} };
    static_assert(sizeof(r1) == sizeof(vi.begin()), "");
    ::models<ranges::concepts::Range>(r1);
    ::models_not<ranges::concepts::SizedRange>(r1);
    if(r1.first != vi.begin()+1) { return false; }
    if(r1.second != ranges::unreachable{}) { return false; }
    r1.second = ranges::unreachable{};

    r0.pop_front();
    if(r0.begin() != vi.begin()+2) { return false; }
    if(r0.size() != 2u) { return false; }
    r0.pop_back();
    if(r0.end() != vi.end()-1) { return false; }
    if(r0.size() != 1u) { return false; }
    if(r0.front() != 3) { return false; }
    if(r0.back() != 3) { return false; }

    std::pair<it, ranges::unreachable> p1 = r1;
    if(p1.first != vi.begin()+1) { return false; }
    if(p1.second != ranges::unreachable{}) { return false; }
    static_assert(sizeof(p1) > sizeof(r1), "");

    ranges::range<it, ranges::unreachable> r2 { p1 };
    if(r1.first != vi.begin()+1) { return false; }
    if(r1.second != ranges::unreachable{}) { return false; }

    auto vi2 = ranges::make_range(vi.begin(), vi.end());
    if(vi2.begin() != vi.begin()) { return false; }
    if(vi2.end() != vi.end()) { return false; }

    if(ranges::get<0>(vi2) != vi.begin()) { return false; }
    if(ranges::get<1>(vi2) != vi.end()) { return false; }

    return true;
}
#endif

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
    ::models<ranges::concepts::Range>(r1);
    ::models_not<ranges::concepts::SizedRange>(r1);
    CHECK(r1.first == vi.begin()+1);
    CHECK(r1.second == ranges::unreachable{});
    r1.second = ranges::unreachable{};

    r0.pop_front();
    CHECK(r0.begin() == vi.begin()+2);
    CHECK(r0.size() == 2u);
    r0.pop_back();
    CHECK(r0.end() == vi.end()-1);
    CHECK(r0.size() == 1u);
    CHECK(r0.front() == 3);
    CHECK(r0.back() == 3);

    std::pair<std::vector<int>::iterator, ranges::unreachable> p1 = r1;
    CHECK(p1.first == vi.begin()+1);
    CHECK(p1.second == ranges::unreachable{});
    static_assert(sizeof(p1) > sizeof(r1), "");

    ranges::range<std::vector<int>::iterator, ranges::unreachable> r2 { p1 };
    CHECK(r1.first == vi.begin()+1);
    CHECK(r1.second == ranges::unreachable{});

    std::list<int> li{1,2,3,4};
    ranges::sized_range<std::list<int>::iterator> l0 {li.begin(), li.end(), li.size()};
    ::models<ranges::concepts::SizedRange>(l0);
    char* sz = test_pop_front(l0); (void) sz;
    CHECK(l0.first == li.begin());
    CHECK(l0.second == li.end());
    CHECK(l0.third == li.size());
    //++l0.first; // disallowed since it would violate the class invariant

    l0 = ranges::view::all(li);

    ranges::range<std::list<int>::iterator> l1 = l0;
    CHECK(l1.first == li.begin());
    CHECK(l1.second == li.end());

#ifdef RANGES_CXX_GREATER_THAN_11
    {
        static_assert(test_constexpr(), "");
    }
#endif

    return ::test_result();
}
