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

#include <range/v3/core.hpp>
#include <range/v3/algorithm/equal.hpp>
#include <range/v3/algorithm/random_shuffle.hpp>
#include <range/v3/numeric/iota.hpp>
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
        int ia[100];
        int ib[100];
        int orig[100];
        ranges::iota(ia, 0);
        ranges::iota(ib, 0);
        ranges::iota(orig, 0);
        const unsigned sa = sizeof(ia)/sizeof(ia[0]);
        ranges::random_shuffle(random_access_iterator<int*>(ia), sentinel<int*>(ia+sa));
        CHECK(!ranges::equal(ia, orig));
        ranges::random_shuffle(ib, ranges::end(ib));
        CHECK(!ranges::equal(ib, orig));
        CHECK(!ranges::equal(ia, ib));
    }

    {
        int ia[100];
        int orig[100];
        ranges::iota(ia, 0);
        ranges::iota(orig, 0);
        ranges::random_shuffle(ia);
        CHECK(!ranges::equal(ia, orig));
    }

    {
        int ia[] = {1, 2, 3, 4};
        const unsigned sa = sizeof(ia)/sizeof(ia[0]);
        gen r;
        ranges::random_shuffle(ia, ia+sa, r);
        check_equal(ia, {3, 1, 2, 4});
    }

    {
        int ia[] = {1, 2, 3, 4};
        const unsigned sa = sizeof(ia)/sizeof(ia[0]);
        gen r;
        auto rng = ranges::make_range(random_access_iterator<int*>(ia), sentinel<int*>(ia+sa));
        CHECK(ranges::random_shuffle(rng, r).base() == ia+sa);
        check_equal(ia, {3, 1, 2, 4});
    }

    {
        int ia[] = {1, 2, 3, 4};
        const unsigned sa = sizeof(ia)/sizeof(ia[0]);
        gen r;
        auto rng = ranges::make_range(random_access_iterator<int*>(ia), sentinel<int*>(ia+sa));
        CHECK(ranges::random_shuffle(std::move(rng), r).get_unsafe().base() == ia+sa);
        check_equal(ia, {3, 1, 2, 4});
    }

    return ::test_result();
}
