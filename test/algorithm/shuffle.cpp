//  Copyright Eric Niebler 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//
//  Copyright 2005 - 2007 Adobe Systems Incorporated
//  Distributed under the MIT License(see accompanying file LICENSE_1_0_0.txt
//  or a copy at http://stlab.adobe.com/licenses.html)

//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include <random>
#include <range/v3/core.hpp>
#include <range/v3/algorithm/equal.hpp>
#include <range/v3/algorithm/shuffle.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"
#include "../test_iterators.hpp"

struct gen
{
    int operator()(int n)
    {
        return n-1;
    }
};

int main()
{
    {
        int ia[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
        const unsigned sa = sizeof(ia)/sizeof(ia[0]);
        std::minstd_rand g;
        ranges::shuffle(random_access_iterator<int*>(ia), sentinel<int*>(ia+sa), g);
        check_equal(ia, {1,2,7,10,4,6,9,3,8,5});
        ranges::shuffle(ia, ia+sa, g);
        check_equal(ia, {3,2,9,1,8,10,4,5,6,7});
    }

    {
        int ia[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
        const unsigned sa = sizeof(ia)/sizeof(ia[0]);
        std::minstd_rand g;
        auto rng = ranges::range(random_access_iterator<int*>(ia), sentinel<int*>(ia+sa));
        ranges::shuffle(rng, g);
        check_equal(ia, {1,2,7,10,4,6,9,3,8,5});
        ranges::shuffle(ia, g);
        check_equal(ia, {3,2,9,1,8,10,4,5,6,7});
    }

    return ::test_result();
}
