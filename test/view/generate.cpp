//  Copyright Eric Niebler 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/

#include <range/v3/core.hpp>
#include <range/v3/view/generate.hpp>
#include <range/v3/view/take.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"

namespace view = ranges::view;

int main()
{
    // Test for constant generator functions
    {
        int i = 0, j = 1;
        auto fib = view::generate([&]()->int{int tmp = i; i += j; std::swap(i, j); return tmp;});
        CONCEPT_ASSERT(ranges::InputIterable<decltype(fib)>());
        check_equal(fib | view::take(10), {0,1,1,2,3,5,8,13,21,34});
        auto const &cfib = fib;
        auto it = fib.begin();
        auto cit = cfib.begin();
        CONCEPT_ASSERT(ranges::Same<decltype(it), decltype(cit)>());
    }

    // Test for mutable-only generator functions
    {
        int i = 0, j = 1;
        auto fib = view::generate([=]()mutable->int{int tmp = i; i += j; std::swap(i, j); return tmp;});
        CONCEPT_ASSERT(ranges::InputIterable<decltype(fib)>());
        check_equal(fib | view::take(10), {0,1,1,2,3,5,8,13,21,34});
        // The generator cannot be called when it's const-qualifies, so "fib const"
        // does not model Iterable.
        CONCEPT_ASSERT(!ranges::Iterable<decltype(fib) const>());
    }

    return test_result();
}
