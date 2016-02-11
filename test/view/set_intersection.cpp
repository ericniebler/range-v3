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

#include <vector>
#include <range/v3/core.hpp>
#include <range/v3/range_for.hpp>
#include <range/v3/algorithm/set_algorithm.hpp>
#include <range/v3/algorithm/move.hpp>
#include <range/v3/utility/iterator.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/view/all.hpp>
#include <range/v3/view/const.hpp>
#include <range/v3/view/drop_while.hpp>
#include <range/v3/view/iota.hpp>
#include <range/v3/view/reverse.hpp>
#include <range/v3/view/set_algorithm.hpp>
#include <range/v3/view/stride.hpp>
#include <range/v3/view/take.hpp>
#include <range/v3/view/transform.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"



int main()
{
    using namespace ranges;

    int i1_finite[] = {1, 2, 2, 3, 3, 3, 4, 4, 4, 4};
    int i2_finite[] = { -3, 2, 4, 4, 6, 9};

    auto i1_infinite = view::ints | view::stride(3);
    auto i2_infinite = view::ints | view::transform([](int x)
    {
        return x * x;
    });


    // intersection of two finite ranges
    {
        auto res = view::set_intersection(i1_finite, i2_finite);

        models<concepts::ForwardView>(res);
        models_not<concepts::RandomAccessView>(res);
        models_not<concepts::BoundedView>(res);

        CONCEPT_ASSERT(Same<range_value_t<decltype(res)>, int>());
        CONCEPT_ASSERT(Same<range_reference_t<decltype(res)>, int&>());
        CONCEPT_ASSERT(Same<decltype(iter_move(begin(res))), int &&> ());

        static_assert(range_cardinality<decltype(res)>::value == ranges::finite, "Cardinality of intersection with a finite range should be finite!");

        ::check_equal(res, {2, 4, 4});
    }


    // intersection of two infinite ranges
    {
        auto res = view::set_intersection(i1_infinite, i2_infinite);

        models<concepts::ForwardView>(res);
        models_not<concepts::RandomAccessView>(res);
        models_not<concepts::BoundedView>(res);

        CONCEPT_ASSERT(Same<range_value_t<decltype(res)>, int>());
        CONCEPT_ASSERT(Same<range_reference_t<decltype(res)>, range_reference_t<decltype(i1_infinite)>>());
        CONCEPT_ASSERT(Same<decltype(iter_move(begin(res))), range_rvalue_reference_t<decltype(i1_infinite)>>());

        static_assert(range_cardinality<decltype(res)>::value == ranges::unknown, "Cardinality of intersection of infinite ranges shold be unknown!");

        ::check_equal(res | view::take(5), {0, 9, 36, 81, 144});
    }


    // intersection of a finite and infinite range
    {
        auto res = view::set_intersection(i1_finite, i2_infinite);

        models<concepts::ForwardView>(res);
        models_not<concepts::RandomAccessView>(res);
        models_not<concepts::BoundedView>(res);

        CONCEPT_ASSERT(Same<range_value_t<decltype(res)>, int>());
        CONCEPT_ASSERT(Same<range_reference_t<decltype(res)>, range_reference_t<decltype(i1_finite)>>());
        CONCEPT_ASSERT(Same<decltype(iter_move(begin(res))), range_rvalue_reference_t<decltype(i1_finite)>>());

        static_assert(range_cardinality<decltype(res)>::value == ranges::finite, "Cardinality of intersection with a finite range shold be finite!");

        ::check_equal(res | view::take(500), {1, 4});



        auto res2 = view::set_intersection(i1_infinite, i2_finite);

        models<concepts::ForwardView>(res2);
        models_not<concepts::RandomAccessView>(res2);
        models_not<concepts::BoundedView>(res2);

        CONCEPT_ASSERT(Same<range_value_t<decltype(res2)>, int>());
        CONCEPT_ASSERT(Same<range_reference_t<decltype(res2)>, range_reference_t<decltype(i1_infinite)>>());
        CONCEPT_ASSERT(Same<range_rvalue_reference_t<decltype(res2)>, range_rvalue_reference_t<decltype(i1_infinite)>>());

        static_assert(range_cardinality<decltype(res2)>::value == ranges::finite, "Cardinality of intersection with a finite range shold be finite!");

        ::check_equal(res2 | view::take(500), {6, 9});
    }


    // intersection of a set of unknown cardinality
    {
        auto rng0 = view::iota(10) | view::drop_while([](int i)
        {
            return i < 25;
        });
        static_assert(range_cardinality<decltype(rng0)>::value == ranges::unknown, "");

        auto res = view::set_intersection(i1_finite, rng0);
        static_assert(range_cardinality<decltype(res)>::value == ranges::unknown, "Intersection with a set of unknown cardinality should have unknown cardinality!");
    }


    // test const ranges
    {
        auto rng1 = view::const_(i1_finite);
        auto rng2 = view::const_(i2_finite);
        auto res = view::set_intersection(rng1, rng2);

        CONCEPT_ASSERT(Same<range_value_t<decltype(res)>, int>());
        CONCEPT_ASSERT(Same<range_reference_t<decltype(res)>, const int&>());
        CONCEPT_ASSERT(Same<range_rvalue_reference_t<decltype(res)>, const int&&> ());
    }


    // test different orderings
    {
        auto res = view::set_intersection(view::reverse(i1_finite), view::reverse(i2_finite), [](int a, int b)
        {
            return a > b;
        });
        ::check_equal(res, {4, 4, 2});
    }


    // test projections and sets with different element types
    struct S
    {
        int val;
        bool operator==(const S& other) const
        {
            return val == other.val;
        }
    };

    S s_finite[] = {S{-20}, S{-10}, S{1}, S{3}, S{3}, S{6}, S{8}, S{20}};
    
    {
        auto res1 = view::set_intersection(s_finite, view::ints(-2, 10),
                                           ordered_less(),
                                           &S::val,
                                           ident()
                                          );
        CONCEPT_ASSERT(Same<range_value_t<decltype(res1)>, S>());
        CONCEPT_ASSERT(Same<range_reference_t<decltype(res1)>, S&>());
        CONCEPT_ASSERT(Same<range_rvalue_reference_t<decltype(res1)>, S&&> ());
        ::check_equal(res1, {S{1}, S{3}, S{6}, S{8}});


        auto res2 = view::set_intersection(view::ints(-2, 10), s_finite,
                                           ordered_less(),
                                           ident(),
                                           [](const S& x){ return x.val; }
                                          );
        CONCEPT_ASSERT(Same<range_value_t<decltype(res2)>, int>());
        ::check_equal(res2, {1, 3, 6, 8});
    }

    return test_result();
}
