// Range v3 library
//
//  Copyright Eric Niebler 2015-present
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
#include <range/v3/view/take.hpp>
#include <range/v3/view/take_exactly.hpp>
#include <range/v3/view/reverse.hpp>
#include <range/v3/view/counted.hpp>
#include <range/v3/view/delimit.hpp>
#include <range/v3/view/c_str.hpp>
#include <range/v3/utility/copy.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"

int main()
{
    using namespace ranges;

    // Reverse a random-access, bounded, sized range
    std::vector<int> rgv{0,1,2,3,4,5,6,7,8,9};
    auto const rng0 = rgv | view::reverse;
    models<ViewConcept>(aux::copy(rng0));
    models<RandomAccessRangeConcept>(rng0);
    models<BoundedRangeConcept>(rng0);
    models<SizedRangeConcept>(rng0);
    CHECK(rng0.size() == 10u);
    ::check_equal(rng0, {9,8,7,6,5,4,3,2,1,0});
    ::check_equal(rng0 | view::reverse, {0,1,2,3,4,5,6,7,8,9});

    // Reverse another random-access, non-bounded, sized range
    auto cnt = view::counted(rgv.begin(), 10);
    models_not<BoundedRangeConcept>(cnt);
    auto const rng1 = rgv | view::reverse;
    models<ViewConcept>(aux::copy(rng1));
    models<RandomAccessRangeConcept>(rng1);
    models<BoundedRangeConcept>(rng1);
    models<SizedRangeConcept>(rng1);
    CHECK(rng1.size() == 10u);
    ::check_equal(rng1, {9,8,7,6,5,4,3,2,1,0});
    ::check_equal(rng1 | view::reverse, {0,1,2,3,4,5,6,7,8,9});

    // Reverse a random-access, non-bounded, non-sized range
    auto sz = view::c_str((char const*)"hello");
    auto rng2 = sz | view::reverse;
    models<ViewConcept>(aux::copy(rng2));
    models<RandomAccessRangeConcept>(rng2);
    models<BoundedRangeConcept>(rng2);
    models_not<SizedRangeConcept>(detail::as_const(rng2));
    models<SizedRangeConcept>(rng2);
    auto const & crng2 = rng2;
    models_not<RangeConcept>(crng2);
    ::check_equal(rng2, {'o','l','l','e','h'});
    ::check_equal(rng2 | view::reverse, {'h','e','l','l','o'});

    // Reverse a bidirectional, bounded, sized range
    std::list<int> rgl{0,1,2,3,4,5,6,7,8,9};
    auto const rng3 = rgl | view::reverse;
    models<ViewConcept>(aux::copy(rng3));
    models<BidirectionalRangeConcept>(rng3);
    models_not<RandomAccessRangeConcept>(rng3);
    models<BoundedRangeConcept>(rng3);
    models<SizedRangeConcept>(rng3);
    CHECK(rng3.size() == 10u);
    ::check_equal(rng3, {9,8,7,6,5,4,3,2,1,0});
    ::check_equal(rng3 | view::reverse, {0,1,2,3,4,5,6,7,8,9});

    // Reverse a bidirectional, weak, sized range
    auto cnt2 = view::counted(rgl.begin(), 10);
    auto rng4 = cnt2 | view::reverse;
    models<ViewConcept>(aux::copy(rng4));
    models<BidirectionalRangeConcept>(rng4);
    models_not<RandomAccessRangeConcept>(rng4);
    models<BoundedRangeConcept>(rng4);
    models<SizedRangeConcept>(rng4);
    CHECK(rng4.size() == 10u);
    auto const & crng4 = rng4;
    models_not<RangeConcept>(crng4);
    ::check_equal(rng4, {9,8,7,6,5,4,3,2,1,0});
    ::check_equal(rng4 | view::reverse, {0,1,2,3,4,5,6,7,8,9});

    // Reverse a bidirectional, weak, non-sized range
    auto dlm = view::delimit(rgl.begin(), 9);
    models_not<BoundedRangeConcept>(dlm);
    auto rng5 = dlm | view::reverse;
    models<ViewConcept>(aux::copy(rng5));
    models<BidirectionalRangeConcept>(rng5);
    models_not<RandomAccessRangeConcept>(rng5);
    models<BoundedRangeConcept>(rng5);
    models_not<SizedRangeConcept>(rng5);
    auto const & crng5 = rng5;
    models_not<RangeConcept>(crng5);
    ::check_equal(rng5, {8,7,6,5,4,3,2,1,0});
    ::check_equal(rng5 | view::reverse, {0,1,2,3,4,5,6,7,8});

    // Reverse a bidirectional, weak, non-sized range
    auto dlm2 = view::delimit(rgl, 10);
    models_not<BoundedRangeConcept>(dlm2);
    auto rng6 = dlm2 | view::reverse;
    models<ViewConcept>(aux::copy(rng6));
    models<BidirectionalRangeConcept>(rng6);
    models_not<RandomAccessRangeConcept>(rng6);
    models<BoundedRangeConcept>(rng6);
    models_not<SizedRangeConcept>(rng6);
    auto const & crng6 = rng6;
    models_not<RangeConcept>(crng6);
    ::check_equal(rng6, {9,8,7,6,5,4,3,2,1,0});
    ::check_equal(rng6 | view::reverse, {0,1,2,3,4,5,6,7,8,9});

    return test_result();
}
