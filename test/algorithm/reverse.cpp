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

#include <utility>
#include <range/v3/core.hpp>
#include <range/v3/algorithm/reverse.hpp>
#include "../safe_int_swap.hpp"
#include "../simple_test.hpp"
#include "../test_utils.hpp"
#include "../test_iterators.hpp"

template <class Iter, class Sent = Iter>
void test()
{
    // iterators
    {
        int ia[] = {0};
        const unsigned sa = sizeof(ia)/sizeof(ia[0]);
        Iter i0 = ranges::reverse(Iter(ia), Sent(ia));
        ::check_equal(ia, {0});
        CHECK(i0 == Iter(ia));
        Iter i1 = ranges::reverse(Iter(ia), Sent(ia+sa));
        ::check_equal(ia, {0});
        CHECK(i1 == Iter(ia+sa));

        int ib[] = {0, 1};
        const unsigned sb = sizeof(ib)/sizeof(ib[0]);
        Iter i2 = ranges::reverse(Iter(ib), Sent(ib+sb));
        ::check_equal(ib, {1, 0});
        CHECK(i2 == Iter(ib+sb));

        int ic[] = {0, 1, 2};
        const unsigned sc = sizeof(ic)/sizeof(ic[0]);
        Iter i3 = ranges::reverse(Iter(ic), Sent(ic+sc));
        ::check_equal(ic, {2, 1, 0});
        CHECK(i3 == Iter(ic+sc));

        int id[] = {0, 1, 2, 3};
        const unsigned sd = sizeof(id)/sizeof(id[0]);
        Iter i4 = ranges::reverse(Iter(id), Sent(id+sd));
        ::check_equal(id, {3, 2, 1, 0});
        CHECK(i4 == Iter(id+sd));
    }

    // ranges
    {
        int ia[] = {0};
        const unsigned sa = sizeof(ia)/sizeof(ia[0]);
        Iter i0 = ranges::reverse(::as_lvalue(ranges::make_range(Iter(ia), Sent(ia))));
        ::check_equal(ia, {0});
        CHECK(i0 == Iter(ia));
        Iter i1 = ranges::reverse(::as_lvalue(ranges::make_range(Iter(ia), Sent(ia+sa))));
        ::check_equal(ia, {0});
        CHECK(i1 == Iter(ia+sa));

        int ib[] = {0, 1};
        const unsigned sb = sizeof(ib)/sizeof(ib[0]);
        Iter i2 = ranges::reverse(::as_lvalue(ranges::make_range(Iter(ib), Sent(ib+sb))));
        ::check_equal(ib, {1, 0});
        CHECK(i2 == Iter(ib+sb));

        int ic[] = {0, 1, 2};
        const unsigned sc = sizeof(ic)/sizeof(ic[0]);
        Iter i3 = ranges::reverse(::as_lvalue(ranges::make_range(Iter(ic), Sent(ic+sc))));
        ::check_equal(ic, {2, 1, 0});
        CHECK(i3 == Iter(ic+sc));

        int id[] = {0, 1, 2, 3};
        const unsigned sd = sizeof(id)/sizeof(id[0]);
        Iter i4 = ranges::reverse(::as_lvalue(ranges::make_range(Iter(id), Sent(id+sd))));
        ::check_equal(id, {3, 2, 1, 0});
        CHECK(i4 == Iter(id+sd));

        // rvalue range
        auto i5 = ranges::reverse(ranges::make_range(Iter(id), Sent(id+sd)));
        ::check_equal(id, {0, 1, 2, 3});
        CHECK(i5.get_unsafe() == Iter(id+sd));
    }
}

#ifdef RANGES_CXX_GREATER_THAN_11
RANGES_CXX14_CONSTEXPR bool test_constexpr()
{
        using namespace ranges;
        safe_int<int>  ia[] = {0, 1, 2, 3, 4};
        constexpr unsigned sa = ranges::size(ia);
        auto r = ranges::reverse(ia);
        if(r != ia + sa) { return false; }
        if(ia[0] != 4) { return false; }
        if(ia[1] != 3) { return false; }
        if(ia[2] != 2) { return false; }
        if(ia[3] != 1) { return false; }
        if(ia[4] != 0) { return false; }
        return true;
}
#endif

int main()
{
    test<bidirectional_iterator<int *>>();
    test<random_access_iterator<int *>>();
    test<int*>();

    test<bidirectional_iterator<int *>, sentinel<int*>>();
    test<random_access_iterator<int *>, sentinel<int*>>();

#ifdef RANGES_CXX_GREATER_THAN_11
    {
            static_assert(test_constexpr(), "");
    }
#endif

    return ::test_result();
}
