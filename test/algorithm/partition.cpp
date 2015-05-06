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

#include <memory>
#include <utility>
#include <range/v3/core.hpp>
#include <range/v3/algorithm/partition.hpp>
#include "../safe_int_swap.hpp"
#include "../simple_test.hpp"
#include "../test_utils.hpp"
#include "../test_iterators.hpp"
#include "../array.hpp"

struct is_odd
{
    constexpr bool operator()(const int& i) const {return i & 1;}
    RANGES_CXX14_CONSTEXPR
    bool operator()(const ranges::safe_int<int>& i) const
    {
        return i % ranges::safe_int<int>(2) != 0;
    }
};

template <class Iter, class Sent = Iter>
void
test_iter()
{
    // check mixed
    int ia[] = {1, 2, 3, 4, 5, 6, 7, 8 ,9};
    const unsigned sa = sizeof(ia)/sizeof(ia[0]);
    Iter r = ranges::partition(Iter(ia), Sent(ia + sa), is_odd());
    CHECK(base(r) == ia + 5);
    for (int* i = ia; i < base(r); ++i)
        CHECK(is_odd()(*i));
    for (int* i = base(r); i < ia+sa; ++i)
        CHECK(!is_odd()(*i));
    // check empty
    r = ranges::partition(Iter(ia), Sent(ia), is_odd());
    CHECK(base(r) == ia);
    // check all false
    for (unsigned i = 0; i < sa; ++i)
        ia[i] = 2*i;
    r = ranges::partition(Iter(ia), Sent(ia+sa), is_odd());
    CHECK(base(r) == ia);
    // check all true
    for (unsigned i = 0; i < sa; ++i)
        ia[i] = 2*i+1;
    r = ranges::partition(Iter(ia), Sent(ia+sa), is_odd());
    CHECK(base(r) == ia+sa);
    // check all true but last
    for (unsigned i = 0; i < sa; ++i)
        ia[i] = 2*i+1;
    ia[sa-1] = 10;
    r = ranges::partition(Iter(ia), Sent(ia+sa), is_odd());
    CHECK(base(r) == ia+sa-1);
    for (int* i = ia; i < base(r); ++i)
        CHECK(is_odd()(*i));
    for (int* i = base(r); i < ia+sa; ++i)
        CHECK(!is_odd()(*i));
    // check all true but first
    for (unsigned i = 0; i < sa; ++i)
        ia[i] = 2*i+1;
    ia[0] = 10;
    r = ranges::partition(Iter(ia), Sent(ia+sa), is_odd());
    CHECK(base(r) == ia+sa-1);
    for (int* i = ia; i < base(r); ++i)
        CHECK(is_odd()(*i));
    for (int* i = base(r); i < ia+sa; ++i)
        CHECK(!is_odd()(*i));
    // check all false but last
    for (unsigned i = 0; i < sa; ++i)
        ia[i] = 2*i;
    ia[sa-1] = 11;
    r = ranges::partition(Iter(ia), Sent(ia+sa), is_odd());
    CHECK(base(r) == ia+1);
    for (int* i = ia; i < base(r); ++i)
        CHECK(is_odd()(*i));
    for (int* i = base(r); i < ia+sa; ++i)
        CHECK(!is_odd()(*i));
    // check all false but first
    for (unsigned i = 0; i < sa; ++i)
        ia[i] = 2*i;
    ia[0] = 11;
    r = ranges::partition(Iter(ia), Sent(ia+sa), is_odd());
    CHECK(base(r) == ia+1);
    for (int* i = ia; i < base(r); ++i)
        CHECK(is_odd()(*i));
    for (int* i = base(r); i < ia+sa; ++i)
        CHECK(!is_odd()(*i));
}

template <class Iter, class Sent = Iter>
void
test_range()
{
    // check mixed
    int ia[] = {1, 2, 3, 4, 5, 6, 7, 8 ,9};
    const unsigned sa = sizeof(ia)/sizeof(ia[0]);
    Iter r = ranges::partition(::as_lvalue(ranges::make_range(Iter(ia), Sent(ia + sa))), is_odd());
    CHECK(base(r) == ia + 5);
    for (int* i = ia; i < base(r); ++i)
        CHECK(is_odd()(*i));
    for (int* i = base(r); i < ia+sa; ++i)
        CHECK(!is_odd()(*i));
    // check empty
    r = ranges::partition(::as_lvalue(ranges::make_range(Iter(ia), Sent(ia))), is_odd());
    CHECK(base(r) == ia);
    // check all false
    for (unsigned i = 0; i < sa; ++i)
        ia[i] = 2*i;
    r = ranges::partition(::as_lvalue(ranges::make_range(Iter(ia), Sent(ia+sa))), is_odd());
    CHECK(base(r) == ia);
    // check all true
    for (unsigned i = 0; i < sa; ++i)
        ia[i] = 2*i+1;
    r = ranges::partition(::as_lvalue(ranges::make_range(Iter(ia), Sent(ia+sa))), is_odd());
    CHECK(base(r) == ia+sa);
    // check all true but last
    for (unsigned i = 0; i < sa; ++i)
        ia[i] = 2*i+1;
    ia[sa-1] = 10;
    r = ranges::partition(::as_lvalue(ranges::make_range(Iter(ia), Sent(ia+sa))), is_odd());
    CHECK(base(r) == ia+sa-1);
    for (int* i = ia; i < base(r); ++i)
        CHECK(is_odd()(*i));
    for (int* i = base(r); i < ia+sa; ++i)
        CHECK(!is_odd()(*i));
    // check all true but first
    for (unsigned i = 0; i < sa; ++i)
        ia[i] = 2*i+1;
    ia[0] = 10;
    r = ranges::partition(::as_lvalue(ranges::make_range(Iter(ia), Sent(ia+sa))), is_odd());
    CHECK(base(r) == ia+sa-1);
    for (int* i = ia; i < base(r); ++i)
        CHECK(is_odd()(*i));
    for (int* i = base(r); i < ia+sa; ++i)
        CHECK(!is_odd()(*i));
    // check all false but last
    for (unsigned i = 0; i < sa; ++i)
        ia[i] = 2*i;
    ia[sa-1] = 11;
    r = ranges::partition(::as_lvalue(ranges::make_range(Iter(ia), Sent(ia+sa))), is_odd());
    CHECK(base(r) == ia+1);
    for (int* i = ia; i < base(r); ++i)
        CHECK(is_odd()(*i));
    for (int* i = base(r); i < ia+sa; ++i)
        CHECK(!is_odd()(*i));
    // check all false but first
    for (unsigned i = 0; i < sa; ++i)
        ia[i] = 2*i;
    ia[0] = 11;
    r = ranges::partition(::as_lvalue(ranges::make_range(Iter(ia), Sent(ia+sa))), is_odd());
    CHECK(base(r) == ia+1);
    for (int* i = ia; i < base(r); ++i)
        CHECK(is_odd()(*i));
    for (int* i = base(r); i < ia+sa; ++i)
        CHECK(!is_odd()(*i));
}

struct S
{
    int i;
};

#ifdef RANGES_CXX_GREATER_THAN_11
RANGES_CXX14_CONSTEXPR bool test_constexpr()
{
    using namespace ranges;
    array<safe_int<int>, 9> ia {{1, 2, 3, 4, 5, 6, 7, 8, 9}};
    safe_int<int>* r = partition(ia, is_odd());
    if(r != begin(ia) + 5) { return false; }
    for (safe_int<int>* i = begin(ia); i < r; ++i)
        if(!is_odd()(*i)) { return false; }
    for (safe_int<int>* i = r; i < end(ia); ++i)
        if(is_odd()(*i)) { return false; }

    //Test rvalue range
    auto r2 = partition(make_range(begin(ia), end(ia)), is_odd());
    if(r2.get_unsafe() != begin(ia) + 5) { return false; }
    for (safe_int<int>* i = begin(ia); i < r2.get_unsafe(); ++i)
        if(!is_odd()(*i)) { return false; }
    for (safe_int<int>* i = r2.get_unsafe(); i < end(ia); ++i)
        if(is_odd()(*i)) { return false; }
    return true;
}
#endif

int main()
{
    test_iter<forward_iterator<int*> >();
    test_iter<bidirectional_iterator<int*> >();
    test_iter<random_access_iterator<int*> >();
    test_iter<int*>();
    test_iter<forward_iterator<int*>, sentinel<int*> >();
    test_iter<bidirectional_iterator<int*>, sentinel<int*> >();
    test_iter<random_access_iterator<int*>, sentinel<int*> >();

    test_range<forward_iterator<int*> >();
    test_range<bidirectional_iterator<int*> >();
    test_range<random_access_iterator<int*> >();
    test_range<int*>();
    test_range<forward_iterator<int*>, sentinel<int*> >();
    test_range<bidirectional_iterator<int*>, sentinel<int*> >();
    test_range<random_access_iterator<int*>, sentinel<int*> >();

    // Test projections
    S ia[] = {S{1}, S{2}, S{3}, S{4}, S{5}, S{6}, S{7}, S{8} ,S{9}};
    const unsigned sa = sizeof(ia)/sizeof(ia[0]);
    S* r = ranges::partition(ia, is_odd(), &S::i);
    CHECK(r == ia + 5);
    for (S* i = ia; i < r; ++i)
        CHECK(is_odd()(i->i));
    for (S* i = r; i < ia+sa; ++i)
        CHECK(!is_odd()(i->i));

    // Test rvalue range
    auto r2 = ranges::partition(ranges::view::all(ia), is_odd(), &S::i);
    CHECK(r2.get_unsafe() == ia + 5);
    for (S* i = ia; i < r2.get_unsafe(); ++i)
        CHECK(is_odd()(i->i));
    for (S* i = r2.get_unsafe(); i < ia+sa; ++i)
        CHECK(!is_odd()(i->i));

#ifdef RANGES_CXX_GREATER_THAN_11
    {
        static_assert(test_constexpr(), "");
    }
#endif

    return ::test_result();
}
