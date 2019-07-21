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
#include <range/v3/view/chunk.hpp>
#include <range/v3/view/drop.hpp>
#include <range/v3/view/iota.hpp>
#include <range/v3/view/join.hpp>
#include <range/v3/view/reverse.hpp>
#include <range/v3/view/take.hpp>
#include <range/v3/view/transform.hpp>
#include <range/v3/utility/copy.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"

int main()
{
    using namespace ranges;

    int rgi[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    auto rng0 = rgi | view::drop(6);
    has_type<int &>(*begin(rng0));
    models<CommonViewConcept>(aux::copy(rng0));
    models<SizedViewConcept>(aux::copy(rng0));
    models<RandomAccessIteratorConcept>(begin(rng0));
    ::check_equal(rng0, {6, 7, 8, 9, 10});
    CHECK(size(rng0) == 5u);

    auto rng1 = rng0 | view::reverse;
    has_type<int &>(*begin(rng1));
    models<CommonViewConcept>(aux::copy(rng1));
    models<SizedViewConcept>(aux::copy(rng1));
    models<RandomAccessIteratorConcept>(begin(rng1));
    ::check_equal(rng1, {10, 9, 8, 7, 6});

    std::vector<int> v{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    auto rng2 = v | view::drop(6) | view::reverse;
    has_type<int &>(*begin(rng2));
    models<CommonViewConcept>(aux::copy(rng2));
    models<SizedViewConcept>(aux::copy(rng2));
    models<RandomAccessIteratorConcept>(begin(rng2));
    ::check_equal(rng2, {10, 9, 8, 7, 6});

    std::list<int> l{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    auto rng3 = l | view::drop(6);
    has_type<int &>(*begin(rng3));
    models<CommonViewConcept>(aux::copy(rng3));
    models<SizedViewConcept>(aux::copy(rng3));
    models<BidirectionalIteratorConcept>(begin(rng3));
    models_not<RandomAccessIteratorConcept>(begin(rng3));
    ::check_equal(rng3, {6, 7, 8, 9, 10});

    auto rng4 = view::iota(10) | view::drop(10);
    ::models<ViewConcept>(aux::copy(rng4));
    ::models_not<CommonViewConcept>(aux::copy(rng4));
    ::models_not<SizedViewConcept>(aux::copy(rng4));
    static_assert(ranges::is_infinite<decltype(rng4)>::value, "");
    auto b = ranges::begin(rng4);
    CHECK(*b == 20);
    CHECK(*(b+1) == 21);

    auto rng5 = view::iota(10) | view::drop(10) | view::take(10) | view::reverse;
    ::models<CommonViewConcept>(aux::copy(rng5));
    ::models<SizedViewConcept>(aux::copy(rng5));
    static_assert(!ranges::is_infinite<decltype(rng5)>::value, "");
    ::check_equal(rng5, {29, 28, 27, 26, 25, 24, 23, 22, 21, 20});
    CHECK(size(rng5) == 10u);

    {
        int some_ints[] = {0,1,2};
        auto rng = make_subrange(some_ints + 0, some_ints + 1);
        auto rng2 = view::drop(rng, 2);
        CHECK(begin(rng2) == some_ints + 1);
        CHECK(size(rng2) == 0u);
    }

    {
        // Regression test for https://github.com/ericniebler/range-v3/issues/413
        auto skips = [](std::vector<int> xs) {
            return view::ints(0, (int)xs.size())
                | view::transform([&](int n) {
                    return xs | view::chunk(n + 1)
                              | view::transform(view::drop(n))
                              | view::join;
                })
                | to<std::vector<std::vector<int>>>();
        };
        auto skipped = skips({1,2,3,4,5,6,7,8});
        CHECK(skipped.size() == 8u);
        if(skipped.size() >= 8u)
        {
            ::check_equal(skipped[0], {1,2,3,4,5,6,7,8});
            ::check_equal(skipped[1], {2,4,6,8});
            ::check_equal(skipped[2], {3,6});
            ::check_equal(skipped[3], {4,8});
            ::check_equal(skipped[4], {5});
            ::check_equal(skipped[5], {6});
            ::check_equal(skipped[6], {7});
            ::check_equal(skipped[7], {8});
        }
    }

    {
        static int const some_ints[] = {0,1,2,3};
        auto rng = debug_input_view<int const>{some_ints} | view::drop(2);
        using R = decltype(rng);
        CPP_assert(InputView<R>);
        CPP_assert(!ForwardRange<R>);
        CPP_assert(Same<int const&, range_reference_t<R>>);
        ::check_equal(rng, {2,3});
    }

    {
        // regression test for #728
        auto rng1 = view::iota(1) | view::chunk(6) | view::take(3);
        int i = 2;
        RANGES_FOR(auto o1, rng1)
        {
            auto rng2 = o1 | view::drop(1);
            ::check_equal(rng2, {i, i+1, i+2, i+3, i+4});
            i += 6;
        }
    }

    {
        // regression test for #813
        static int const some_ints[] = {0,1,2,3};
        auto rng = some_ints | view::drop(10);
        CHECK(empty(rng));
    }

    return test_result();
}
