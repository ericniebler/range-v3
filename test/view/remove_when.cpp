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
#include <string>

#include <range/v3/utility/copy.hpp>
#include <range/v3/view/remove_when.hpp>

#include "../simple_test.hpp"
#include "../test_utils.hpp"

int main()
{
    using namespace ranges;

    // a simple predicate behaves just like a worse remove_if
    std::string str = "The  quick  brown   fox";
    auto rng0 = view::remove_when(str, [](char c) { return c == ' '; });
    ::check_equal(rng0, std::string("Thequickbrownfox"));

    // a silly predicate that actually doesn't remove anything
    std::list<int> lst = {1, 2, 3, 4, 5};
    auto rng1 =
        view::remove_when(lst, [](auto i, auto) { return std::make_pair(true, i); });
    ::check_equal(rng1, {1, 2, 3, 4, 5});

    // remove 3s and what's after 3
    auto rng2 = view::remove_when(
        lst, [](auto i, auto) { return std::make_pair(*i == 3, next(i, 2)); });
    ::check_equal(rng2, {1, 2, 5});
}
