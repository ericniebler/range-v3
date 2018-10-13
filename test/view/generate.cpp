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
#include <range/v3/view/generate.hpp>
#include <range/v3/view/take_exactly.hpp>
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
        auto fib = view::generate([&]()->int{int tmp = i; i += j; std::swap(i, j); return tmp;});
        CONCEPT_ASSERT(ranges::InputView<decltype(fib)>());
        check_equal(fib | view::take_exactly(10), {0,1,1,2,3,5,8,13,21,34});
    }

    // Test for mutable-only generator functions
    {
        int i = 0, j = 1;
        auto fib = view::generate([=]()mutable->int{int tmp = i; i += j; std::swap(i, j); return tmp;});
        CONCEPT_ASSERT(ranges::InputView<decltype(fib)>());
        check_equal(fib | view::take_exactly(10), {0,1,1,2,3,5,8,13,21,34});
        // The generator cannot be called when it's const-qualified, so "fib const"
        // does not model View.
        CONCEPT_ASSERT(!ranges::View<decltype(fib) const>());
    }

    // Test for move-only generator functions
    {
        auto rng = view::generate(MoveOnlyFunction{"Hello, world!", 0}) | view::take_exactly(5);
        CONCEPT_ASSERT(ranges::InputView<decltype(rng)>());
        check_equal(rng, {'H', 'e', 'l', 'l', 'o'});
    }

    // Test for generator functions that return move-only types
    // https://github.com/ericniebler/range-v3/issues/905
    {
        char str[] = "gi";
        auto rng = view::generate([&]{str[0]++; return MoveOnlyString{str};}) | view::take_exactly(2);
        auto i = rng.begin();
        CHECK(bool(*i == MoveOnlyString{"hi"}));
        CHECK(bool(*i == MoveOnlyString{"hi"}));
        CHECK(bool(*rng.begin() == MoveOnlyString{"hi"}));
        CHECK(bool(*rng.begin() == MoveOnlyString{"hi"}));
        CONCEPT_ASSERT(ranges::InputView<decltype(rng)>());
        check_equal(rng, {MoveOnlyString{"hi"}, MoveOnlyString{"ii"}});
        static_assert(std::is_same<ranges::range_reference_t<decltype(rng)>, MoveOnlyString &&>::value, "");
    }

    // Test for generator functions that return internal references
    // https://github.com/ericniebler/range-v3/issues/807
    {
        int i = 42;
        auto rng = view::generate([i]{return &i;});
        auto rng2 = std::move(rng);
        auto it = rng2.begin();
        auto p = *it;
        auto p2 = *++it;
        CHECK(p == p2);
    }

    // Test that we only call the function once for each dereferenceable position
    // https://github.com/ericniebler/range-v3/issues/819
    {
        int i = 0;
        auto rng = view::generate([&i]{return ++i;});
        auto rng2 = std::move(rng);
        auto it = rng2.begin();
        CHECK(i == 0);
        CHECK(*it == 1);
        CHECK(i == 1);
        ++it;
        CHECK(i == 1);
        CHECK(*it == 2);
        CHECK(i == 2);
    }

    return test_result();
}
