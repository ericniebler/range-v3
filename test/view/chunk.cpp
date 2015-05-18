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
#include <forward_list>
#include <range/v3/core.hpp>
#include <range/v3/view/iota.hpp>
#include <range/v3/view/chunk.hpp>
#include <range/v3/view/reverse.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"

int main()
{
    using namespace ranges;

    std::vector<int> v = view::iota(0,11);
    auto rng1 = v | view::chunk(3);
    ::models<concepts::RandomAccessRange>(rng1);
    ::models<concepts::SizedRange>(rng1);
    auto it1 = ranges::begin(rng1);
    ::check_equal(*it1++, {0,1,2});
    ::check_equal(*it1++, {3,4,5});
    ::check_equal(*it1++, {6,7,8});
    ::check_equal(*it1++, {9,10});
    CHECK(it1 == ranges::end(rng1));
    ::check_equal(*ranges::next(it1, -3), {3,4,5});
    CHECK(size(rng1), 4u);
    static_assert(sizeof(rng1.begin()) == sizeof(v.begin())*2+sizeof(std::ptrdiff_t)*2, "");

    std::forward_list<int> l = view::iota(0,11);
    auto rng2 = l | view::chunk(3);
    ::models<concepts::ForwardRange>(rng2);
    ::models_not<concepts::BidirectionalRange>(rng2);
    ::models_not<concepts::SizedRange>(rng2);
    auto it2 = ranges::begin(rng2);
    ::check_equal(*it2++, {0,1,2});
    ::check_equal(*it2++, {3,4,5});
    ::check_equal(*it2++, {6,7,8});
    ::check_equal(*it2++, {9,10});
    CHECK(it2 == ranges::end(rng2));
    static_assert(sizeof(rng2.begin()) == sizeof(l.begin())*2+sizeof(std::ptrdiff_t), "");

    return ::test_result();
}
