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
#include <range/v3/view/counted.hpp>
#include <range/v3/view/group_by.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"
#include "../test_iterators.hpp"

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
        auto rng0 = v | view::group_by([](P p0, P p1) {return p0.second == p1.second;});
        CONCEPT_ASSERT(ForwardIterable<decltype(rng0)>());
        CONCEPT_ASSERT(!BidirectionalIterable<decltype(rng0)>());
        CHECK(distance(rng0) == 3);
        check_equal(*rng0.begin(), {P{1,1},P{1,1}});
        check_equal(*next(rng0.begin()), {P{1,2},P{1,2},P{1,2},P{1,2},P{2,2},P{2,2}});
        check_equal(*next(rng0.begin(), 2), {P{2,3},P{2,3},P{2,3},P{2,3}});

        auto rng1 = v | view::group_by([](P p0, P p1) {return p0.first == p1.first;});
        CONCEPT_ASSERT(ForwardIterable<decltype(rng1)>());
        CONCEPT_ASSERT(!BidirectionalIterable<decltype(rng1)>());
        CHECK(distance(rng1) == 2);
        check_equal(*rng1.begin(), {P{1,1},P{1,1},P{1,2},P{1,2},P{1,2},P{1,2}});
        check_equal(*next(rng1.begin()), {P{2,2},P{2,2},P{2,3},P{2,3},P{2,3},P{2,3}});
    }

    {
        forward_iterator<std::vector<P>::iterator> b{v.begin()};
        auto rng0 = view::counted(b, v.size())
            | view::group_by([](P p0, P p1) {return p0.second == p1.second;});
        CONCEPT_ASSERT(ForwardIterable<decltype(rng0)>());
        CONCEPT_ASSERT(!BidirectionalIterable<decltype(rng0)>());
        CHECK(distance(rng0) == 3);
        check_equal(*rng0.begin(), {P{1,1},P{1,1}});
        check_equal(*next(rng0.begin()), {P{1,2},P{1,2},P{1,2},P{1,2},P{2,2},P{2,2}});
        check_equal(*next(rng0.begin(), 2), {P{2,3},P{2,3},P{2,3},P{2,3}});

        auto rng1 = view::counted(b, v.size())
            | view::group_by([](P p0, P p1) {return p0.first == p1.first;});
        CONCEPT_ASSERT(ForwardIterable<decltype(rng1)>());
        CONCEPT_ASSERT(!BidirectionalIterable<decltype(rng1)>());
        CHECK(distance(rng1) == 2);
        check_equal(*rng1.begin(), {P{1,1},P{1,1},P{1,2},P{1,2},P{1,2},P{1,2}});
        check_equal(*next(rng1.begin()), {P{2,2},P{2,2},P{2,3},P{2,3},P{2,3},P{2,3}});
    }

    return test_result();
}
