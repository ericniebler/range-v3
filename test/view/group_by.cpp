// Range v3 library
//
//  Copyright Eric Niebler 2014-present
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
#include <range/v3/view/counted.hpp>
#include <range/v3/view/group_by.hpp>
#include <range/v3/view/remove_if.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"
#include "../test_iterators.hpp"

RANGES_DIAGNOSTIC_IGNORE_SIGN_CONVERSION

int main()
{
    using namespace ranges;
    using P = std::pair<int,int>;

    std::vector<std::pair<int, int>> v =
    {
        {1,1},
        {1,1},
        {1,2},
        {1,2},
        {1,2},
        {1,2},
        {2,2},
        {2,2},
        {2,3},
        {2,3},
        {2,3},
        {2,3}
    };

    {
        auto rng0 = v | views::group_by([](P p0, P p1) {return p0.second == p1.second;});
        CPP_assert(forward_range<decltype(rng0)>);
        CPP_assert(!bidirectional_range<decltype(rng0)>);
        CHECK(distance(rng0) == 3);
        check_equal(*rng0.begin(), {P{1,1},P{1,1}});
        check_equal(*next(rng0.begin()), {P{1,2},P{1,2},P{1,2},P{1,2},P{2,2},P{2,2}});
        check_equal(*next(rng0.begin(), 2), {P{2,3},P{2,3},P{2,3},P{2,3}});

        auto rng1 = v | views::group_by([](P p0, P p1) {return p0.first == p1.first;});
        CPP_assert(forward_range<decltype(rng1)>);
        CPP_assert(!bidirectional_range<decltype(rng1)>);
        CHECK(distance(rng1) == 2);
        check_equal(*rng1.begin(), {P{1,1},P{1,1},P{1,2},P{1,2},P{1,2},P{1,2}});
        check_equal(*next(rng1.begin()), {P{2,2},P{2,2},P{2,3},P{2,3},P{2,3},P{2,3}});
    }

    {
        ForwardIterator<std::vector<P>::iterator> b{v.begin()};
        auto rng0 = views::counted(b, v.size())
            | views::group_by([](P p0, P p1) {return p0.second == p1.second;});
        CPP_assert(forward_range<decltype(rng0)>);
        CPP_assert(!bidirectional_range<decltype(rng0)>);
        CHECK(distance(rng0) == 3);
        check_equal(*rng0.begin(), {P{1,1},P{1,1}});
        check_equal(*next(rng0.begin()), {P{1,2},P{1,2},P{1,2},P{1,2},P{2,2},P{2,2}});
        check_equal(*next(rng0.begin(), 2), {P{2,3},P{2,3},P{2,3},P{2,3}});

        auto rng1 = views::counted(b, v.size())
            | views::group_by([](P p0, P p1) {return p0.first == p1.first;});
        CPP_assert(forward_range<decltype(rng1)>);
        CPP_assert(!bidirectional_range<decltype(rng1)>);
        CHECK(distance(rng1) == 2);
        check_equal(*rng1.begin(), {P{1,1},P{1,1},P{1,2},P{1,2},P{1,2},P{1,2}});
        check_equal(*next(rng1.begin()), {P{2,2},P{2,2},P{2,3},P{2,3},P{2,3},P{2,3}});
    }

    {
        int a[] = {0, 1, 2, 3, 4, 5};
        auto rng = a | views::remove_if([](int n) { return n % 2 == 0; })
          | views::group_by([](int, int) { return true; });
        check_equal(*rng.begin(), {1, 3, 5});
    }

    {
        std::vector<int> v2 {0,1,2,3,4,5,6,7,8,9};
        auto rng0 = ranges::views::group_by(v2, [](int i, int j){ return j - i < 3; });
        check_equal(*rng0.begin(), {0, 1, 2});
        check_equal(*next(rng0.begin()), {3, 4, 5});
        check_equal(*next(rng0.begin(), 2), {6, 7, 8});
        check_equal(*next(rng0.begin(), 3), {9});
        CHECK(distance(rng0) == 4);
    }

    return test_result();
}
