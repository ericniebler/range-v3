// Range v3 library
//
//  Copyright Tomislav Ivek 2015-2016
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3

#include <string>
#include <vector>
#include <iterator>
#include <functional>
#include <range/v3/core.hpp>
#include <range/v3/range_for.hpp>
#include <range/v3/view/const.hpp>
#include <range/v3/view/iota.hpp>
#include <range/v3/view/set_algorithm.hpp>
#include <range/v3/view/transform.hpp>
#include <range/v3/algorithm/set_algorithm.hpp>
#include <range/v3/algorithm/move.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"



int main()
{
    using namespace ranges;

    int ia[] = {1, 2, 2, 3, 3, 3, 4, 4, 4, 4};
    int ib[] = {2, 4, 4, 6};
    int ir[] = {2, 4, 4};
    
    float fa[] = {-10, 1.5, 2.0, 3.5, 4.0, 5.0, 6.5, 7.0, 8.0, 9.5, 10.0};

    {
        auto res = view::set_intersection(ia, ib);

        models<concepts::ForwardView>(res);
        models_not<concepts::RandomAccessView>(res);
        CONCEPT_ASSERT(Same<range_value_t<decltype(res)>, int>());
        CONCEPT_ASSERT(Same<range_reference_t<decltype(res)>, int &>());
        CONCEPT_ASSERT(Same<decltype(iter_move(begin(res))), int &&>());

        ::check_equal(res, ir);
        CHECK(*begin(res) == *(begin(ia) + 1));
    }
    
    {
        auto res = view::set_intersection(view::const_(ia), ib);

        CONCEPT_ASSERT(Same<range_value_t<decltype(res)>, int>());
        CONCEPT_ASSERT(Same<range_reference_t<decltype(res)>, const int &>());

        ::check_equal(res, ir);
        CHECK(*begin(res) == *(begin(ia) + 1));
    }
    
    {
        auto res = view::set_intersection(ia, fa); // IEE754 can represent all integers exactly as floats, up to about 2^23

        CONCEPT_ASSERT(Same<range_value_t<decltype(res)>, int>());
        CONCEPT_ASSERT(Same<decltype(iter_move(begin(res))), int &&>());

        ::check_equal(res, {2, 4});
    }
    
    {
        auto res = view::set_intersection(fa, ia);

        CONCEPT_ASSERT(Same<range_value_t<decltype(res)>, float>());
        CONCEPT_ASSERT(Same<decltype(iter_move(begin(res))), float &&>());

        ::check_equal(res, {2.0, 4.0});
    }
    
    {
        auto res = view::set_intersection(view::ints(-10, 11), view::ints(-2, 10) | view::transform([](int x){ return x * 3; }));

        CONCEPT_ASSERT(Same<range_value_t<decltype(res)>, int>());
        
        ::check_equal(res, {-6, -3, 0, 3, 6, 9});
    }
    
    // test projections
    {
        struct S
        {
            int val;
        };
        
        S sa[] = {S{-20}, S{-10}, S{1}, S{3}, S{8}, S{20}};
        S stest[] = {S{1}, S{3}, S{8}};
        auto res = view::set_intersection(sa, view::ints(-2, 10),
                                          std::less<int>(),
                                          &S::val,
                                          ident()
                                         );

        CONCEPT_ASSERT(Same<range_value_t<decltype(res)>, S>());
        
        ::check_equal(res, stest);
    }

    {
        struct S
        {
            int val;
        };
        
        S sa[] = {-20, -10, 1, 3, 8, 20};
        auto res = view::set_intersection(view::ints(-2, 10), sa,
                                          std::less<int>(),
                                          ident(),
                                          &S::val
                                         );

        CONCEPT_ASSERT(Same<range_value_t<decltype(res)>, int>());
        
        ::check_equal(res, {1, 3, 8});
    }


    return test_result();
}
