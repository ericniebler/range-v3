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

#include <range/v3/core.hpp>
#include <range/v3/view/generate_n.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"

namespace view = ranges::view;

struct MoveOnlyFunction
{
    MoveOnlyString str_;
    int i_;

    char operator()()
    {
        return str_.sz_[i_++];
    }
};

int main()
{
    // Test for constant generator functions
    {
        int i = 0, j = 1;
        auto fib = view::generate_n([&]()->int{int tmp = i; i += j; std::swap(i, j); return tmp;}, 10);
        CONCEPT_ASSERT(ranges::InputView<decltype(fib)>());
        check_equal(fib, {0,1,1,2,3,5,8,13,21,34});
    }

    // Test for mutable-only generator functions
    {
        int i = 0, j = 1;
        auto fib = view::generate_n([=]()mutable->int{int tmp = i; i += j; std::swap(i, j); return tmp;}, 10);
        CONCEPT_ASSERT(ranges::InputView<decltype(fib)>());
        check_equal(fib, {0,1,1,2,3,5,8,13,21,34});
        // The generator cannot be called when it's const-qualified, so "fib const"
        // does not model View.
        CONCEPT_ASSERT(!ranges::View<decltype(fib) const>());
    }

    // Test for move-only generator functions
    {
        auto rng = view::generate_n(MoveOnlyFunction{"Hello, world!", 0}, 5);
        CONCEPT_ASSERT(ranges::InputView<decltype(rng)>());
        check_equal(rng, {'H', 'e', 'l', 'l', 'o'});
    }

    return test_result();
}
