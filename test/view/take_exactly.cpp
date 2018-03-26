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
#include <range/v3/view/iota.hpp>
#include <range/v3/view/take_exactly.hpp>
#include <range/v3/view/reverse.hpp>
#include <range/v3/utility/copy.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"

int main()
{
    using namespace ranges;

    int rgi[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    auto rng0 = rgi | view::take_exactly(6);
    has_type<int &>(*begin(rng0));
    models<concepts::BoundedView>(aux::copy(rng0));
    models<concepts::SizedView>(aux::copy(rng0));
    models<concepts::RandomAccessIterator>(begin(rng0));
    ::check_equal(rng0, {0, 1, 2, 3, 4, 5});
    CHECK(size(rng0) == 6u);

    auto rng1 = rng0 | view::reverse;
    has_type<int &>(*begin(rng1));
    models<concepts::BoundedView>(aux::copy(rng1));
    models<concepts::SizedView>(aux::copy(rng1));
    models<concepts::RandomAccessIterator>(begin(rng1));
    ::check_equal(rng1, {5, 4, 3, 2, 1, 0});

    std::vector<int> v{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    auto rng2 = v | view::take_exactly(6) | view::reverse;
    has_type<int &>(*begin(rng2));
    models<concepts::BoundedView>(aux::copy(rng2));
    models<concepts::SizedView>(aux::copy(rng2));
    models<concepts::RandomAccessIterator>(begin(rng2));
    ::check_equal(rng2, {5, 4, 3, 2, 1, 0});

    std::list<int> l{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    auto rng3 = l | view::take_exactly(6);
    has_type<int &>(*begin(rng3));
    models<concepts::View>(aux::copy(rng3));
    models_not<concepts::BoundedView>(aux::copy(rng3));
    models<concepts::SizedView>(aux::copy(rng3));
    models<concepts::BidirectionalIterator>(begin(rng3));
    models_not<concepts::RandomAccessIterator>(begin(rng3));
    ::check_equal(rng3, {0, 1, 2, 3, 4, 5});

    auto rng4 = view::iota(10) | view::take_exactly(10);
    ::models_not<concepts::BoundedView>(aux::copy(rng4));
    ::models<concepts::SizedView>(aux::copy(rng4));
    static_assert(!ranges::is_infinite<decltype(rng4)>::value, "");
    ::check_equal(rng4, {10, 11, 12, 13, 14, 15, 16, 17, 18, 19});
    CHECK(size(rng4) == 10u);

    auto rng5 = view::iota(10) | view::take_exactly(10) | view::reverse;
    ::models<concepts::BoundedView>(aux::copy(rng5));
    ::models<concepts::SizedView>(aux::copy(rng5));
    static_assert(!ranges::is_infinite<decltype(rng5)>::value, "");
    ::check_equal(rng5, {19, 18, 17, 16, 15, 14, 13, 12, 11, 10});
    CHECK(size(rng5) == 10u);

    {
        auto rng = debug_input_view<int const>{rgi} | view::take_exactly(6);
        ::check_equal(rng, {0, 1, 2, 3, 4, 5});
    }

    return test_result();
}
