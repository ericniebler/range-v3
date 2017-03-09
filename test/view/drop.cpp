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
    models<concepts::BoundedView>(aux::copy(rng0));
    models<concepts::SizedView>(aux::copy(rng0));
    models<concepts::RandomAccessIterator>(begin(rng0));
    ::check_equal(rng0, {6, 7, 8, 9, 10});
    CHECK(size(rng0) == 5u);

    auto rng1 = rng0 | view::reverse;
    has_type<int &>(*begin(rng1));
    models<concepts::BoundedView>(aux::copy(rng1));
    models<concepts::SizedView>(aux::copy(rng1));
    models<concepts::RandomAccessIterator>(begin(rng1));
    ::check_equal(rng1, {10, 9, 8, 7, 6});

    std::vector<int> v{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    auto rng2 = v | view::drop(6) | view::reverse;
    has_type<int &>(*begin(rng2));
    models<concepts::BoundedView>(aux::copy(rng2));
    models<concepts::SizedView>(aux::copy(rng2));
    models<concepts::RandomAccessIterator>(begin(rng2));
    ::check_equal(rng2, {10, 9, 8, 7, 6});

    std::list<int> l{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    auto rng3 = l | view::drop(6);
    has_type<int &>(*begin(rng3));
    models<concepts::BoundedView>(aux::copy(rng3));
    models<concepts::SizedView>(aux::copy(rng3));
    models<concepts::BidirectionalIterator>(begin(rng3));
    models_not<concepts::RandomAccessIterator>(begin(rng3));
    ::check_equal(rng3, {6, 7, 8, 9, 10});

    auto rng4 = view::iota(10) | view::drop(10);
    ::models<concepts::View>(aux::copy(rng4));
    ::models_not<concepts::BoundedView>(aux::copy(rng4));
    ::models_not<concepts::SizedView>(aux::copy(rng4));
    static_assert(ranges::is_infinite<decltype(rng4)>::value, "");
    auto b = ranges::begin(rng4);
    CHECK(*b == 20);
    CHECK(*(b+1) == 21);

    auto rng5 = view::iota(10) | view::drop(10) | view::take(10) | view::reverse;
    ::models<concepts::BoundedView>(aux::copy(rng5));
    ::models<concepts::SizedView>(aux::copy(rng5));
    static_assert(!ranges::is_infinite<decltype(rng5)>::value, "");
    ::check_equal(rng5, {29, 28, 27, 26, 25, 24, 23, 22, 21, 20});
    CHECK(size(rng5) == 10u);

    {
        int some_ints[] = {0,1,2};
        auto rng = make_iterator_range(some_ints + 0, some_ints + 1);
        auto rng2 = view::drop(rng, 2);
        CHECK(begin(rng2) == some_ints + 1);
        CHECK(size(rng2) == 0u);
    }

    {
        // Regression test for https://github.com/ericniebler/range-v3/issues/413
        auto skips = [](std::vector<int> xs) -> std::vector<std::vector<int>> {
            return view::ints(0, (int)xs.size())
                | view::transform([&](int n) {
                    return xs | view::chunk(n + 1)
                              | view::transform(view::drop(n))
                              | view::join;
                });
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
    return test_result();
}
