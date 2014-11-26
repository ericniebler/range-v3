// Range v3 library
//
//  Copyright Eric Niebler 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <set>
#include <vector>
#include <range/v3/core.hpp>
#include <range/v3/view/iota.hpp>
#include <range/v3/view/take.hpp>
#include <range/v3/view/for_each.hpp>
#include <range/v3/action/insert.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"

int main()
{
    using namespace ranges;

    std::vector<int> v;
    auto i = insert(v, v.begin(), 42);
    CHECK(i == v.begin());
    ::check_equal(v, {42});
    insert(v, v.end(), {1,2,3});
    ::check_equal(v, {42,1,2,3});

    insert(v, v.begin(), view::ints | view::take(3));
    ::check_equal(v, {0,1,2,42,1,2,3});

    int rg[] = {9,8,7};
    insert(v, v.begin()+3, rg);
    ::check_equal(v, {0,1,2,9,8,7,42,1,2,3});
    insert(v, v.begin()+1, rg);
    ::check_equal(v, {0,9,8,7,1,2,9,8,7,42,1,2,3});

    std::set<int> s;
    insert(s,
        view::ints|view::take(10)|view::for_each([](int i){return yield_if(i%2==0,i);}));
    ::check_equal(s, {0,2,4,6,8});
    auto j = insert(s, 10);
    CHECK(j.first == prev(s.end()));
    CHECK(j.second == true);
    ::check_equal(s, {0,2,4,6,8,10});

    insert(std::ref(s), 12);
    ::check_equal(s, {0,2,4,6,8,10,12});

    insert(ranges::ref(s), 14);
    ::check_equal(s, {0,2,4,6,8,10,12,14});

    return ::test_result();
}
