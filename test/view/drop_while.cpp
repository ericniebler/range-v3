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
#include <range/v3/core.hpp>
#include <range/v3/view/iota.hpp>
#include <range/v3/view/take.hpp>
#include <range/v3/view/drop_while.hpp>
#include <range/v3/utility/copy.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"

struct my_data
{
    int i;
};

bool operator==(my_data left, my_data right)
{
    return left.i == right.i;
}

int main()
{
    using namespace ranges;

    auto rng0 = view::iota(10) | view::drop_while([](int i) { return i < 25; });
    CONCEPT_ASSERT(range_cardinality<decltype(rng0)>::value == unknown);
    ::models<concepts::RandomAccessView>(aux::copy(rng0));
    ::models_not<concepts::BoundedView>(aux::copy(rng0));
    ::models<concepts::RandomAccessIterator>(rng0.begin());
    auto b = rng0.begin();
    CHECK(*b == 25);
    CHECK(*(b+1) == 26);
    ::check_equal(rng0 | view::take(10), {25, 26, 27, 28, 29, 30, 31, 32, 33, 34});

    std::list<int> vi{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    auto rng1 = vi | view::drop_while([](int i) { return i != 50; });
    CONCEPT_ASSERT(range_cardinality<decltype(rng1)>::value == ranges::finite);
    ::models<concepts::BidirectionalView>(aux::copy(rng1));
    ::models<concepts::BoundedView>(aux::copy(rng1));
    ::models<concepts::BidirectionalIterator>(rng1.begin());
    CHECK(rng1.begin() == rng1.end());

    // Check with a mutable predicate
    static int const rgi[] = {0,1,2,3,4,5,6,7,8,9};
    int cnt = 0;
    auto mutable_only = view::drop_while(rgi, [cnt](int) mutable { return ++cnt <= 5;});
    ::check_equal(mutable_only, {5,6,7,8,9});
    CONCEPT_ASSERT(View<decltype(mutable_only)>());
    CONCEPT_ASSERT(!View<decltype(mutable_only) const>());

    {
        // Check with move-only subview
        auto rng = debug_input_view<const int>{rgi} | view::drop_while([](int i){ return i < 4; });
        using R = decltype(rng);
        CONCEPT_ASSERT(InputView<R>());
        CONCEPT_ASSERT(!ForwardRange<R>());
        CONCEPT_ASSERT(!BoundedRange<R>());
        CONCEPT_ASSERT(Same<int const&, range_reference_t<R>>());
        ::check_equal(rng, {4,5,6,7,8,9});
    }

    {
        // with projection
        const std::list<my_data> data_list{{1}, {2}, {3}, {1}};
        auto rng = data_list | view::drop_while([](int i){ return i <= 2; }, &my_data::i);
        ::check_equal(rng, std::list<my_data>{{3}, {1}});
    }

    return test_result();
}
