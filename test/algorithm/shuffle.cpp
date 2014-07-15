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
#include <range/v3/algorithm/shuffle.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"
#include "../test_iterators.hpp"

int main()
{
    {
        static const constexpr unsigned N = 10;
        int a[N] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
        int a_ref[N] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

        std::minstd_rand g_a;
        std::minstd_rand g_a_ref;

        ranges::shuffle(random_access_iterator<int*>(a), sentinel<int*>(a+N), g_a);
        std::shuffle(std::begin(a_ref), std::end(a_ref), g_a_ref);
        check_equal(a, a_ref);

        ranges::shuffle(random_access_iterator<int*>(a), sentinel<int*>(a+N), g_a);
        std::shuffle(std::begin(a_ref), std::end(a_ref), g_a_ref);
        check_equal(a, a_ref);
    }

    {
        static const constexpr unsigned N = 10;
        int a[N] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
        int a_ref[N] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

        std::minstd_rand g_a;
        std::minstd_rand g_a_ref;

        auto rng = ranges::range(random_access_iterator<int*>(a), sentinel<int*>(a+N));

        ranges::shuffle(rng, g_a);
        std::shuffle(std::begin(a_ref), std::end(a_ref), g_a_ref);
        check_equal(a, a_ref);

        ranges::shuffle(rng, g_a);
        std::shuffle(std::begin(a_ref), std::end(a_ref), g_a_ref);
        check_equal(a, a_ref);
    }

    return ::test_result();
}
