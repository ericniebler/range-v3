/// \file
// Range v3 library
//
//  Copyright Andrey Diduh 2019
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#include <vector>

#include <range/v3/view/addressof.hpp>

#include "../simple_test.hpp"
#include "../test_utils.hpp"

using namespace ranges;

void simple_test()
{
    std::vector<int> list = {1,2,3};

    auto out = list | view::addressof;

    check_equal(out, {&list[0], &list[1], &list[2]});
}

int main()
{
    simple_test();
    return test_result();
}
