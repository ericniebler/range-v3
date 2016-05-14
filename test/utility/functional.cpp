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

#include <range/v3/utility/functional.hpp>
#include <range/v3/view/filter.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"

struct Integer
{
    int i;
    operator int() const { return i; }
    bool odd() const { return (i % 2) != 0; }
};

int main()
{
    using namespace ranges;

    {
      // Check that not_ works with callables
      Integer some_ints[] = {{0}, {1}, {2}, {3}, {4}, {5}, {6}, {7}};
      ::check_equal(some_ints | view::filter(ranges::not_(&Integer::odd)),
                    {0,2,4,6});
    }

    return test_result();
}
