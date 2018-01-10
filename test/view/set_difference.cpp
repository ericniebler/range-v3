// Range v3 library
//
//  Copyright Eric Niebler 2014-present
//  Copyright Tomislav Ivek 2015-2016
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3

#include <vector>
#include <sstream>
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
#include <range/v3/view/move.hpp>
#include <range/v3/view/reverse.hpp>
#include <range/v3/view/set_algorithm.hpp>
#include <range/v3/view/stride.hpp>
#include <range/v3/view/take.hpp>
#include <range/v3/view/transform.hpp>
#include <range/v3/utility/copy.hpp>
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

    // difference between two finite ranges/sets
    {
        auto res = view::set_difference(i1_finite, i2_finite);

        models<concepts::ForwardView>(aux::copy(res));
        models_not<concepts::RandomAccessView>(aux::copy(res));
        models_not<concepts::BoundedView>(aux::copy(res));

        using R = decltype(res);

        CONCEPT_ASSERT(Same<range_value_type_t<R>, int>());
        CONCEPT_ASSERT(Same<range_reference_t<R>, int&>());
        CONCEPT_ASSERT(Same<decltype(iter_move(begin(res))), int &&> ());

        static_assert(range_cardinality<R>::value == ranges::finite, "Cardinality of difference between two finite ranges should be finite!");

        ::check_equal(res, {1, 2, 3, 3, 3, 4, 4});

        // check if the final result agrees with the greedy algorithm
        std::vector<int> diff;
        set_difference(i1_finite, i2_finite, back_inserter(diff));
        ::check_equal(res, diff);

        CHECK(&*begin(res) == &*(begin(i1_finite)));
    }

    // difference between two infinite ranges
    {
        auto res = view::set_difference(i1_infinite, i2_infinite);

        models<concepts::ForwardView>(aux::copy(res));
        models_not<concepts::RandomAccessView>(aux::copy(res));
        models_not<concepts::BoundedView>(aux::copy(res));

        using R = decltype(res);

        CONCEPT_ASSERT(Same<range_value_type_t<R>, int>());
        CONCEPT_ASSERT(Same<range_reference_t<R>, range_reference_t<decltype(i1_infinite)>>());
        CONCEPT_ASSERT(Same<decltype(iter_move(begin(res))), range_rvalue_reference_t<decltype(i1_infinite)>>());

        static_assert(range_cardinality<R>::value == ranges::unknown, "Cardinality of difference of infinite ranges should be unknown!");

        ::check_equal(res | view::take(5), {3, 6, 12, 15, 18});

        // check if the final result agrees with the greedy algorithm
        std::vector<int> diff;
        set_difference(i1_infinite | view::take(1000), i2_infinite | view::take(1000), back_inserter(diff));
        ::check_equal(res | view::take(5), diff | view::take(5));

    }

    // difference between a finite and an infinite range
    {
        auto res = view::set_difference(i1_finite, i2_infinite);

        models<concepts::ForwardView>(aux::copy(res));
        models_not<concepts::RandomAccessView>(aux::copy(res));
        models_not<concepts::BoundedView>(aux::copy(res));

        using R = decltype(res);

        CONCEPT_ASSERT(Same<range_value_type_t<R>, int>());
        CONCEPT_ASSERT(Same<range_reference_t<R>, range_reference_t<decltype(i1_finite)>>());
        CONCEPT_ASSERT(Same<decltype(iter_move(begin(res))), range_rvalue_reference_t<decltype(i1_finite)>>());

        static_assert(range_cardinality<R>::value == ranges::finite, "Cardinality of difference between a finite range and any other range should be finite!");

        ::check_equal(res, {2, 2, 3, 3, 3, 4, 4, 4});
    }

    // difference between an infinite and a finite range
    {
        auto res = view::set_difference(i1_infinite, i2_finite);

        models<concepts::ForwardView>(aux::copy(res));
        models_not<concepts::RandomAccessView>(aux::copy(res));
        models_not<concepts::BoundedView>(aux::copy(res));

        using R = decltype(res);

        CONCEPT_ASSERT(Same<range_value_type_t<R>, int>());
        CONCEPT_ASSERT(Same<range_reference_t<R>, range_reference_t<decltype(i1_infinite)>>());
        CONCEPT_ASSERT(Same<range_rvalue_reference_t<R>, range_rvalue_reference_t<decltype(i1_infinite)>>());

        static_assert(range_cardinality<R>::value == ranges::infinite, "Cardinality of difference between an infinite and finite range should be infinite!");

        ::check_equal(res | view::take(5), {0, 3, 12, 15, 18});
    }

    // differences involving unknown cardinalities
    {
        auto rng0 = view::iota(10) | view::drop_while([](int i)
        {
            return i < 25;
        });
        static_assert(range_cardinality<decltype(rng0)>::value == ranges::unknown, "");

        auto res1 = view::set_difference(i2_finite, rng0);
        static_assert(range_cardinality<decltype(res1)>::value == ranges::finite, "Difference between a finite and unknown cardinality set should have finite cardinality!");

        auto res2 = view::set_difference(rng0, i2_finite);
        static_assert(range_cardinality<decltype(res2)>::value == ranges::unknown, "Difference between an unknown cardinality and finite set should have unknown cardinality!");

        auto res3 = view::set_difference(i1_infinite, rng0);
        static_assert(range_cardinality<decltype(res3)>::value == ranges::unknown, "Difference between an unknown cardinality and finite set should have unknown cardinality!");

        auto res4 = view::set_difference(rng0, i1_infinite);
        static_assert(range_cardinality<decltype(res4)>::value == ranges::unknown, "Difference between an unknown and infinite cardinality set should have unknown cardinality!");

    }

    // test const ranges
    {
        auto res1 = view::set_difference(view::const_(i1_finite), view::const_(i2_finite));
        using R1 = decltype(res1);
        CONCEPT_ASSERT(Same<range_value_type_t<R1>, int>());
        CONCEPT_ASSERT(Same<range_reference_t<R1>, const int&>());
        CONCEPT_ASSERT(Same<range_rvalue_reference_t<R1>, const int&&> ());

        auto res2 = view::set_difference(view::const_(i1_finite), i2_finite);
        using R2 = decltype(res2);
        CONCEPT_ASSERT(Same<range_value_type_t<R2>, int>());
        CONCEPT_ASSERT(Same<range_reference_t<R2>, const int&>());
        CONCEPT_ASSERT(Same<range_rvalue_reference_t<R2>, const int&&> ());
    }

    // test different orderings
    {
        auto res = view::set_difference(view::reverse(i1_finite), view::reverse(i2_finite), [](int a, int b)
        {
            return a > b;
        });
        ::check_equal(res, {4, 4, 3, 3, 3, 2, 1});
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
        auto res1 = view::set_difference(s_finite, view::ints(-2, 10),
                                         ordered_less(),
                                         &S::val,
                                         ident()
                                        );
        using R1 = decltype(res1);
        CONCEPT_ASSERT(Same<range_value_type_t<R1>, S>());
        CONCEPT_ASSERT(Same<range_reference_t<R1>, S&>());
        CONCEPT_ASSERT(Same<range_rvalue_reference_t<R1>, S&&> ());
        ::check_equal(res1, {S{-20}, S{-10}, S{3}, S{20}});

        auto res2 = view::set_difference(view::ints(-2, 10), s_finite,
                                         ordered_less(),
                                         ident(),
                                         [](const S& x){ return x.val; }
                                        );
        using R2 = decltype(res2);
        CONCEPT_ASSERT(Same<range_value_type_t<R2>, int>());
        CONCEPT_ASSERT(Same<range_reference_t<R2>, int>());
        CONCEPT_ASSERT(Same<range_rvalue_reference_t<R2>, int> ());
        ::check_equal(res2, {-2, -1, 0, 2, 4, 5, 7, 9});
    }

    // move
    {
        auto v0 = to_<std::vector<MoveOnlyString>>({"a","b","b","c","x","x"});
        auto v1 = to_<std::vector<MoveOnlyString>>({"b","x","y","z"});
        auto res = view::set_difference(v0, v1, [](const MoveOnlyString& a, const MoveOnlyString& b){return a<b;});

        std::vector<MoveOnlyString> expected;
        move(res, back_inserter(expected));

        ::check_equal(expected, {"a","b","c","x"});
        ::check_equal(v1, {"b","x","y","z"});
        ::check_equal(v0, {"","b","","","x",""});

        auto v0_greedy = to_<std::vector<MoveOnlyString>>({"a","b","b","c","x","x"});
        auto v1_greedy = to_<std::vector<MoveOnlyString>>({"b","x","y","z"});
        std::vector<MoveOnlyString> expected_greedy;
        set_difference(v0_greedy, v1_greedy,
                       move_into(back_inserter(expected_greedy)),
                       [](const MoveOnlyString& a, const MoveOnlyString& b){return a<b;});
        ::check_equal(expected_greedy, expected);
        ::check_equal(v0_greedy, v0);
        ::check_equal(v1_greedy, v1);

        using R = decltype(res);

        CONCEPT_ASSERT(Same<range_value_type_t<R>, MoveOnlyString>());
        CONCEPT_ASSERT(Same<range_reference_t<R>, MoveOnlyString &>());
        CONCEPT_ASSERT(Same<range_rvalue_reference_t<R>, MoveOnlyString &&>());
    }

    // WARNING: set_difference between two infinite ranges can create infinite loops!
    // {
    //     auto empty_range = view::set_difference(view::ints, view::ints);
    //     begin(empty_range); // infinite loop!
    // }

    {
        auto rng = view::set_difference(
            debug_input_view<int const>{i1_finite},
            debug_input_view<int const>{i2_finite}
        );
        ::check_equal(rng, {1, 2, 3, 3, 3, 4, 4});
    }

    return test_result();
}
