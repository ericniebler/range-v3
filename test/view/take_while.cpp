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

#include <vector>
#include <range/v3/core.hpp>
#include <range/v3/view/iota.hpp>
#include <range/v3/view/generate.hpp>
#include <range/v3/view/take_while.hpp>
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
    auto rng0 = view::iota(10) | view::take_while([](int i) { return i != 25; });
    ::check_equal(rng0, {10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24});
    ::models<concepts::View>(aux::copy(rng0));
    ::models_not<concepts::BoundedView>(aux::copy(rng0));
    ::models<concepts::RandomAccessIterator>(rng0.begin());

    std::vector<int> vi{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    auto rng1 = vi | view::take_while([](int i) { return i != 50; });
    ::models<concepts::RandomAccessView>(aux::copy(rng1));
    ::check_equal(rng1, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9});

    // Check with a mutable predicate
    int rgi[] = {0,1,2,3,4,5,6,7,8,9};
    int cnt = 0;
    auto mutable_only = view::take_while(rgi, [cnt](int) mutable { return ++cnt <= 5;});
    ::check_equal(mutable_only, {0,1,2,3,4});
    CONCEPT_ASSERT(View<decltype(mutable_only)>());
    CONCEPT_ASSERT(!View<decltype(mutable_only) const>());

    {
        auto ns = view::generate([]() mutable {
            static int N;
            return ++N;
        });
        auto rng = ns | view::take_while([](int i) { return i < 5; });
        ::check_equal(rng, {1,2,3,4});
    }

    {
        auto ns = view::generate([]() mutable {
            static int N;
            return ++N;
        });
        auto rng = ns | view::take_while([](int i) mutable { return i < 5; });
        ::check_equal(rng, {1,2,3,4});
    }

    {
        auto rng = debug_input_view<int const>{rgi} | view::take_while([](int i) {
            return i != 5;
        });
        ::check_equal(rng, {0,1,2,3,4});
    }

    {
        auto ns = view::generate([]() {
            static int N;
            return my_data{++N};
        });
        auto rng = ns | view::take_while([](int i) { return i < 5; },
                                         &my_data::i);
        ::check_equal(rng, std::vector<my_data>{{1},{2},{3},{4}});
    }

    return test_result();
}
