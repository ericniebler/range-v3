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

#include <cstring>
#include <utility>
#include <range/v3/core.hpp>
#include <range/v3/algorithm/reverse_copy.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"
#include "../test_iterators.hpp"

template<class Iter, class OutIter, class Sent = Iter>
void test()
{
    using P = std::pair<Iter, OutIter>;
    // iterators
    {
        const int ia[] = {0};
        const unsigned sa = sizeof(ia)/sizeof(ia[0]);
        int ja[sa] = {-1};
        P p0 = ranges::reverse_copy(Iter(ia), Sent(ia), OutIter(ja));
        ::check_equal(ja, {-1});
        CHECK(p0.first == Iter(ia));
        CHECK(base(p0.second) == ja);
        P p1 = ranges::reverse_copy(Iter(ia), Sent(ia+sa), OutIter(ja));
        ::check_equal(ja, {0});
        CHECK(p1.first == Iter(ia+sa));
        CHECK(base(p1.second) == ja+sa);

        const int ib[] = {0, 1};
        const unsigned sb = sizeof(ib)/sizeof(ib[0]);
        int jb[sb] = {-1};
        P p2 = ranges::reverse_copy(Iter(ib), Sent(ib+sb), OutIter(jb));
        ::check_equal(jb, {1, 0});
        CHECK(p2.first == Iter(ib+sb));
        CHECK(base(p2.second) == jb+sb);

        const int ic[] = {0, 1, 2};
        const unsigned sc = sizeof(ic)/sizeof(ic[0]);
        int jc[sc] = {-1};
        P p3 = ranges::reverse_copy(Iter(ic), Sent(ic+sc), OutIter(jc));
        ::check_equal(jc, {2, 1, 0});
        CHECK(p3.first == Iter(ic+sc));
        CHECK(base(p3.second) == jc+sc);

        const int id[] = {0, 1, 2, 3};
        const unsigned sd = sizeof(id)/sizeof(id[0]);
        int jd[sd] = {-1};
        P p4 = ranges::reverse_copy(Iter(id), Sent(id+sd), OutIter(jd));
        ::check_equal(jd, {3, 2, 1, 0});
        CHECK(p4.first == Iter(id+sd));
        CHECK(base(p4.second) == jd+sd);
    }

    // ranges
    {
        const int ia[] = {0};
        const unsigned sa = sizeof(ia)/sizeof(ia[0]);
        int ja[sa] = {-1};
        P p0 = ranges::reverse_copy(::as_lvalue(ranges::make_iterator_range(Iter(ia), Sent(ia))), OutIter(ja));
        ::check_equal(ja, {-1});
        CHECK(p0.first == Iter(ia));
        CHECK(base(p0.second) == ja);
        P p1 = ranges::reverse_copy(::as_lvalue(ranges::make_iterator_range(Iter(ia), Sent(ia+sa))), OutIter(ja));
        ::check_equal(ja, {0});
        CHECK(p1.first == Iter(ia+sa));
        CHECK(base(p1.second) == ja+sa);

        const int ib[] = {0, 1};
        const unsigned sb = sizeof(ib)/sizeof(ib[0]);
        int jb[sb] = {-1};
        P p2 = ranges::reverse_copy(::as_lvalue(ranges::make_iterator_range(Iter(ib), Sent(ib+sb))), OutIter(jb));
        ::check_equal(jb, {1, 0});
        CHECK(p2.first == Iter(ib+sb));
        CHECK(base(p2.second) == jb+sb);

        const int ic[] = {0, 1, 2};
        const unsigned sc = sizeof(ic)/sizeof(ic[0]);
        int jc[sc] = {-1};
        P p3 = ranges::reverse_copy(::as_lvalue(ranges::make_iterator_range(Iter(ic), Sent(ic+sc))), OutIter(jc));
        ::check_equal(jc, {2, 1, 0});
        CHECK(p3.first == Iter(ic+sc));
        CHECK(base(p3.second) == jc+sc);

        const int id[] = {0, 1, 2, 3};
        const unsigned sd = sizeof(id)/sizeof(id[0]);
        int jd[sd] = {-1};
        P p4 = ranges::reverse_copy(::as_lvalue(ranges::make_iterator_range(Iter(id), Sent(id+sd))), OutIter(jd));
        ::check_equal(jd, {3, 2, 1, 0});
        CHECK(p4.first == Iter(id+sd));
        CHECK(base(p4.second) == jd+sd);

        // test rvalue ranges
        std::memset(jd, 0, sizeof(jd));
        auto p5 = ranges::reverse_copy(ranges::make_iterator_range(Iter(id), Sent(id+sd)), OutIter(jd));
        ::check_equal(jd, {3, 2, 1, 0});
        CHECK(p5.first.get_unsafe() == Iter(id+sd));
        CHECK(base(p4.second) == jd+sd);
    }
}

int main()
{
    test<bidirectional_iterator<const int*>, output_iterator<int*> >();
    test<bidirectional_iterator<const int*>, forward_iterator<int*> >();
    test<bidirectional_iterator<const int*>, bidirectional_iterator<int*> >();
    test<bidirectional_iterator<const int*>, random_access_iterator<int*> >();
    test<bidirectional_iterator<const int*>, int*>();

    test<random_access_iterator<const int*>, output_iterator<int*> >();
    test<random_access_iterator<const int*>, forward_iterator<int*> >();
    test<random_access_iterator<const int*>, bidirectional_iterator<int*> >();
    test<random_access_iterator<const int*>, random_access_iterator<int*> >();
    test<random_access_iterator<const int*>, int*>();

    test<const int*, output_iterator<int*> >();
    test<const int*, forward_iterator<int*> >();
    test<const int*, bidirectional_iterator<int*> >();
    test<const int*, random_access_iterator<int*> >();
    test<const int*, int*>();

    test<bidirectional_iterator<const int*>, output_iterator<int*>, sentinel<const int *> >();
    test<bidirectional_iterator<const int*>, forward_iterator<int*>, sentinel<const int *> >();
    test<bidirectional_iterator<const int*>, bidirectional_iterator<int*>, sentinel<const int *> >();
    test<bidirectional_iterator<const int*>, random_access_iterator<int*>, sentinel<const int *> >();
    test<bidirectional_iterator<const int*>, int*>();

    test<random_access_iterator<const int*>, output_iterator<int*>, sentinel<const int *> >();
    test<random_access_iterator<const int*>, forward_iterator<int*>, sentinel<const int *> >();
    test<random_access_iterator<const int*>, bidirectional_iterator<int*>, sentinel<const int *> >();
    test<random_access_iterator<const int*>, random_access_iterator<int*>, sentinel<const int *> >();
    test<random_access_iterator<const int*>, int*>();

    test<const int*, output_iterator<int*>, sentinel<const int *> >();
    test<const int*, forward_iterator<int*>, sentinel<const int *> >();
    test<const int*, bidirectional_iterator<int*>, sentinel<const int *> >();
    test<const int*, random_access_iterator<int*>, sentinel<const int *> >();
    test<const int*, int*>();

    return ::test_result();
}
