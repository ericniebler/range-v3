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
//

// This example shows how to define a range that is implemented
// in terms of itself. The example is generating the Fibonacci
// sequence using self-reference and zip_with.
//
// Note: don't use recursive_range_fn in performance sensitive
// code. Self-reference comes with indirection and dynamic
// allocation overhead.

#include "./recursive_range.hpp"
#include <iostream>
#include <range/v3/all.hpp>

int
main()
{
    using namespace ranges::view;

    // Define a nullary function fibs that returns an infinite range
    // that generates the Fibonacci sequence.
    ranges::ext::recursive_range_fn<int> const fibs{[&] {
        return concat(closed_ints(0, 1),
                      zip_with(std::plus<int>{}, fibs(), tail(fibs())));
    }};

    auto x = take(fibs(), 20);
    ranges::for_each(x, [](int i) { std::cout << i << std::endl; });
}
