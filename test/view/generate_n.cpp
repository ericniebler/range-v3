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

#include <range/v3/core.hpp>
#include <range/v3/view/generate_n.hpp>
#include <range/v3/algorithm/equal.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"

namespace view = ranges::view;

#ifdef RANGES_CXX_GREATER_THAN_11
struct gen {
    int i;
    int j;
    RANGES_RELAXED_CONSTEXPR gen() : i(0), j(1) {}
    RANGES_RELAXED_CONSTEXPR gen(gen const&) = default;
    RANGES_RELAXED_CONSTEXPR gen(gen &&) = default;
    RANGES_RELAXED_CONSTEXPR gen& operator=(gen const&) = default;
    RANGES_RELAXED_CONSTEXPR gen& operator=(gen &&) = default;

    RANGES_RELAXED_CONSTEXPR int operator()() {
        int tmp = i;
        i += j;
        int tmp2 = i;
        i = j;
        j = tmp2;
        // std::swap(i, j); // not constexpr
        return tmp;
    }
};

#endif

int main()
{
    // Test for constant generator functions
    {
        int i = 0, j = 1;
        auto fib = view::generate_n([&]()->int{int tmp = i; i += j; std::swap(i, j); return tmp;}, 10);
        CONCEPT_ASSERT(ranges::InputRange<decltype(fib)>());
        check_equal(fib, {0,1,1,2,3,5,8,13,21,34});
    }

    // Test for mutable-only generator functions
    {
        int i = 0, j = 1;
        auto fib = view::generate_n([=]()mutable->int{int tmp = i; i += j; std::swap(i, j); return tmp;}, 10);
        CONCEPT_ASSERT(ranges::InputRange<decltype(fib)>());
        check_equal(fib, {0,1,1,2,3,5,8,13,21,34});
        // The generator cannot be called when it's const-qualified, so "fib const"
        // does not model Range.
        CONCEPT_ASSERT(!ranges::Range<decltype(fib) const>());
    }

#ifdef RANGES_CXX_GREATER_THAN_11
    {
        static_assert(ranges::equal(view::generate_n(gen(), 10),  {0,1,1,2,3,5,8,13,21,34}), "");
        static_assert(!ranges::equal(view::generate_n(gen(), 10),  {0,1,1,2,3,5,8,13,22,34}), "");
    }
#endif

    return test_result();
}
