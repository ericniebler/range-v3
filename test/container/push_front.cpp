// Range v3 library
//
//  Copyright Eric Niebler 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <list>
#include <vector>
#include <range/v3/core.hpp>
#include <range/v3/view/iota.hpp>
#include <range/v3/view/take.hpp>
#include <range/v3/view/for_each.hpp>
#include <range/v3/container/push_front.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"

int main()
{
    using namespace ranges;

    std::vector<int> v;
    push_front(v, {1,2,3});
    ::check_equal(v, {1,2,3});

    push_front(v, view::ints(10) | view::take(3));
    ::check_equal(v, {10,11,12,1,2,3});

    view::ints(10) | view::take(3) | push_front(v);
    ::check_equal(v, {10,11,12,10,11,12,1,2,3});

    int rg[] = {9,8,7};
    push_front(v, rg);
    ::check_equal(v, {9,8,7,10,11,12,10,11,12,1,2,3});
    rg | push_front(v);
    ::check_equal(v, {9,8,7,9,8,7,10,11,12,10,11,12,1,2,3});

    std::list<int> s;
    push_front(s,
        view::ints|view::take(10)|view::for_each([](int i){return yield_if(i%2==0,i);}));
    ::check_equal(s, {0,2,4,6,8});
    push_front(s, -2);
    ::check_equal(s, {-2,0,2,4,6,8});

    return ::test_result();
}
