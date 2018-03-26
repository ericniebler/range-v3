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
#include <range/v3/utility/common_type.hpp>
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

    // simple identity check
    {
        ::check_equal(view::set_union(i1_infinite, i1_infinite) | view::take(100), i1_infinite | view::take(100));
    }

    // union of two finite ranges/sets
    {
        auto res = view::set_union(i1_finite, i2_finite);

        models<concepts::ForwardView>(aux::copy(res));
        models_not<concepts::RandomAccessView>(aux::copy(res));
        models_not<concepts::BoundedView>(aux::copy(res));

        using R = decltype(res);

        CONCEPT_ASSERT(Same<range_value_type_t<R>, int>());
        CONCEPT_ASSERT(Same<range_reference_t<R>, int&>());
        CONCEPT_ASSERT(Same<decltype(iter_move(begin(res))), int&&>());

        static_assert(range_cardinality<R>::value == ranges::finite, "Cardinality of union of finite ranges should be finite!");

        ::check_equal(res, {-3, 1, 2, 2, 3, 3, 3, 4, 4, 4, 4, 6, 9});

        // check if the final result agrees with the greedy algorithm
        std::vector<int> greedy_union;
        set_union(i1_finite, i2_finite, back_inserter(greedy_union));
        ::check_equal(res, greedy_union);

        auto it = begin(res);
        CHECK(&*it == &*(begin(i2_finite)));
        ++it;
        CHECK(&*it == &*(begin(i1_finite)));
    }

    // union of two infinite ranges
    {
        auto res = view::set_union(i1_infinite, i2_infinite);

        models<concepts::ForwardView>(aux::copy(res));
        models_not<concepts::RandomAccessView>(aux::copy(res));
        models_not<concepts::BoundedView>(aux::copy(res));

        using R = decltype(res);

        CONCEPT_ASSERT(Same<range_value_type_t<R>,
                            common_type_t<range_value_type_t<decltype(i1_infinite)>,
                                          range_value_type_t<decltype(i2_infinite)>>>());
        CONCEPT_ASSERT(Same<range_reference_t<R>,
                            common_reference_t<range_reference_t<decltype(i1_infinite)>,
                                               range_reference_t<decltype(i2_infinite)>>
                           >());
        CONCEPT_ASSERT(Same<range_rvalue_reference_t<R>,
                            common_reference_t<range_rvalue_reference_t<decltype(i1_infinite)>,
                                               range_rvalue_reference_t<decltype(i2_infinite)>>
                           >());

        static_assert(range_cardinality<R>::value == ranges::infinite, "Cardinality of union of infinite ranges should be infinite!");

        ::check_equal(res | view::take(6), {0, 1, 3, 4, 6, 9});

        // check if the final result agrees with the greedy algorithm
        std::vector<int> greedy_union;
        set_union(i1_infinite | view::take(10), i2_infinite | view::take(10), back_inserter(greedy_union));
        ::check_equal(res | view::take(6), greedy_union | view::take(6));
    }

    // union of a finite and an infinite range
    {
        auto res = view::set_union(i1_finite, i2_infinite);

        models<concepts::ForwardView>(aux::copy(res));
        models_not<concepts::RandomAccessView>(aux::copy(res));
        models_not<concepts::BoundedView>(aux::copy(res));

        using R = decltype(res);

        CONCEPT_ASSERT(Same<range_value_type_t<R>, int>());
        CONCEPT_ASSERT(Same<range_reference_t<R>, int>()); // our infinite range does not give out references
        CONCEPT_ASSERT(Same<range_rvalue_reference_t<R>, int>());

        static_assert(range_cardinality<R>::value == ranges::infinite, "Cardinality of union with an infinite range should be infinite!");

        ::check_equal(res | view::take(5), {0, 1, 2, 2, 3});
    }

    // union of an infinite and a finite range
    {
        auto res = view::set_union(i1_infinite, i2_finite);

        models<concepts::ForwardView>(aux::copy(res));
        models_not<concepts::RandomAccessView>(aux::copy(res));
        models_not<concepts::BoundedView>(aux::copy(res));

        using R = decltype(res);

        CONCEPT_ASSERT(Same<range_value_type_t<R>, int>());
        CONCEPT_ASSERT(Same<range_reference_t<R>, int>()); // our infinite range does not give out references
        CONCEPT_ASSERT(Same<range_rvalue_reference_t<R>, int>());

        static_assert(range_cardinality<R>::value == ranges::infinite, "Cardinality of union with an infinite range should be infinite!");

        ::check_equal(res | view::take(7), {-3, 0, 2, 3, 4, 4, 6});
    }

    // unions involving unknown cardinalities
    {
        auto rng0 = view::iota(10) | view::drop_while([](int i)
        {
            return i < 25;
        });
        static_assert(range_cardinality<decltype(rng0)>::value == ranges::unknown, "");

        auto res1 = view::set_union(i2_finite, rng0);
        static_assert(range_cardinality<decltype(res1)>::value == ranges::unknown, "Union of a finite and unknown cardinality set should have unknown cardinality!");

        auto res2 = view::set_union(rng0, i2_finite);
        static_assert(range_cardinality<decltype(res2)>::value == ranges::unknown, "Union of an unknown and finite cardinality set should have unknown cardinality!");

        auto res3 = view::set_union(i1_infinite, rng0);
        static_assert(range_cardinality<decltype(res3)>::value == ranges::infinite, "Union of an infinite and unknown cardinality set should have infinite cardinality!");

        auto res4 = view::set_union(rng0, i1_infinite);
        static_assert(range_cardinality<decltype(res4)>::value == ranges::infinite, "Union of an unknown and infinite cardinality set should have infinite cardinality!");

        auto res5 = view::set_union(rng0, rng0);
        static_assert(range_cardinality<decltype(res5)>::value == ranges::unknown, "Union of two unknown cardinality sets should have unknown cardinality!");
        ::check_equal(res5 | view::take(100), rng0 | view::take(100));
    }

    // test const ranges
    {
        auto res1 = view::set_union(view::const_(i1_finite), view::const_(i2_finite));
        using R1 = decltype(res1);
        CONCEPT_ASSERT(Same<range_value_type_t<R1>, int>());
        CONCEPT_ASSERT(Same<range_reference_t<R1>, const int&>());
        CONCEPT_ASSERT(Same<range_rvalue_reference_t<R1>, const int&&>());

        auto res2 = view::set_union(view::const_(i1_finite), i2_finite);
        using R2 = decltype(res2);
        CONCEPT_ASSERT(Same<range_value_type_t<R2>, int>());
        CONCEPT_ASSERT(Same<range_reference_t<R2>, const int&>());
        CONCEPT_ASSERT(Same<range_rvalue_reference_t<R2>, const int&&>());
    }

    // test different orderings
    {
        auto res = view::set_union(view::reverse(i1_finite), view::reverse(i2_finite), [](int a, int b)
        {
            return a > b;
        });
        ::check_equal(res, {9, 6, 4, 4, 4, 4, 3, 3, 3, 2, 2, 1, -3});
    }

    struct B
    {
        int val;
        B(int i): val{i} {}
        bool operator==(const B& other) const
        {
            return val == other.val;
        }
    };

    struct D: public B
    {
        D(int i): B{i} {}
        D(B b): B{std::move(b)} {}
    };

    B b_finite[] = {B{-20}, B{-10}, B{1}, B{3}, B{3}, B{6}, B{8}, B{20}};
    D d_finite[] = {D{0}, D{2}, D{4}, D{6}};

    // sets with different element types, custom orderings
    {
        auto res = view::set_union(b_finite, d_finite, [](const B& a, const D& b){ return a.val < b.val; });
        using R = decltype(res);
        CONCEPT_ASSERT(Same<range_value_type_t<R>, B>());
        CONCEPT_ASSERT(Same<range_reference_t<R>, B&>());
        CONCEPT_ASSERT(Same<range_rvalue_reference_t<R>, B&&>());
        ::check_equal(res, {B{-20}, B{-10}, B{0}, B{1}, B{2}, B{3}, B{3}, B{4}, B{6}, B{8}, B{20}});
        auto it = begin(res);
        CHECK(&*it == &*begin(b_finite));
        advance(it, 2);
        CHECK(&*it == &*begin(d_finite));
    }

    // projections
    {
        auto res1 = view::set_union(b_finite, d_finite,
                                    ordered_less(),
                                    &B::val,
                                    &D::val
                                   );
        using R1 = decltype(res1);
        CONCEPT_ASSERT(Same<range_value_type_t<R1>, B>());
        CONCEPT_ASSERT(Same<range_reference_t<R1>, B&>());
        CONCEPT_ASSERT(Same<range_rvalue_reference_t<R1>, B&&>());
        ::check_equal(res1, {B{-20}, B{-10}, B{0}, B{1}, B{2}, B{3}, B{3}, B{4}, B{6}, B{8}, B{20}});

        auto res2 = view::set_union(view::ints(-2, 10), b_finite,
                                    ordered_less(),
                                    ident(),
                                    [](const B& x){ return x.val; }
                                   );
        using R2 = decltype(res2);
        CONCEPT_ASSERT(Same<range_value_type_t<R2>, B>());
        CONCEPT_ASSERT(Same<range_reference_t<R2>, B>());
        CONCEPT_ASSERT(Same<range_rvalue_reference_t<R2>, B>());
        ::check_equal(res2, {B{-20}, B{-10}, B{-2}, B{-1}, B{0}, B{1}, B{2}, B{3}, B{3}, B{4}, B{5}, B{6}, B{7}, B{8}, B{9}, B{20}});
    }

    // move
    {
        auto v0 = to_<std::vector<MoveOnlyString>>({"a","b","c","x"});
        auto v1 = to_<std::vector<MoveOnlyString>>({"b","x","y","z"});
        auto res = view::set_union(v0, v1, [](const MoveOnlyString& a, const MoveOnlyString& b){return a<b;});

        std::vector<MoveOnlyString> expected;
        move(res, back_inserter(expected));

        ::check_equal(expected, {"a","b","c","x","y","z"});
        ::check_equal(v0, {"","","",""});
        ::check_equal(v1, {"b","x","",""});

        using R = decltype(res);

        CONCEPT_ASSERT(Same<range_value_type_t<R>, MoveOnlyString>());
        CONCEPT_ASSERT(Same<range_reference_t<R>, MoveOnlyString &>());
        CONCEPT_ASSERT(Same<range_rvalue_reference_t<R>, MoveOnlyString &&>());
    }

    // iterator (in)equality
    {
        int r1[] = {1, 2, 3};
        int r2[] = {   2, 3, 4, 5};
        auto res = view::set_union(r1, r2); // 1, 2, 3, 4, 5

        auto it1 = ranges::next(res.begin(), 3); // *it1 == 4, member iterator into r1 points to r1.end()
        auto it2 = ranges::next(it1);            // *it2 == 5, member iterator into r1 also points to r1.end()
        auto sentinel = res.end();

        CHECK(*it1 == 4);
        CHECK(*it2 == 5);

        CHECK(it1 != it2); // should be different even though member iterators into r1 are the same

        CHECK(it1 != sentinel);
        CHECK(ranges::next(it1, 2) == sentinel);

        CHECK(it2 != sentinel);
        CHECK(ranges::next(it2, 1) == sentinel);
    }

    {
        auto rng = view::set_union(
            debug_input_view<int const>{i1_finite},
            debug_input_view<int const>{i2_finite}
        );
        ::check_equal(rng, {-3, 1, 2, 2, 3, 3, 3, 4, 4, 4, 4, 6, 9});
    }

    return test_result();
}
