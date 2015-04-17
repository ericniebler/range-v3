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
#include <algorithm>
#include <functional>
#include <range/v3/core.hpp>
#include <range/v3/algorithm/heap_algorithm.hpp>
#include <range/v3/utility/array.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"
#include "../test_iterators.hpp"

void test_1(int N)
{
    int* ia = new int [N];
    for (int i = 0; i < N; ++i)
        ia[i] = i;
    std::random_shuffle(ia, ia+N);
    std::make_heap(ia, ia+N);
    for (int i = N; i > 0; --i)
    {
        CHECK(ranges::pop_heap(ia, ia+i) == ia+i);
        CHECK(std::is_heap(ia, ia+i-1));
    }
    CHECK(ranges::pop_heap(ia, ia) == ia);
    delete [] ia;
}

void test_2(int N)
{
    int* ia = new int [N];
    for (int i = 0; i < N; ++i)
        ia[i] = i;
    std::random_shuffle(ia, ia+N);
    std::make_heap(ia, ia+N);
    for (int i = N; i > 0; --i)
    {
        CHECK(ranges::pop_heap(ia, sentinel<int*>(ia+i)) == ia+i);
        CHECK(std::is_heap(ia, ia+i-1));
    }
    CHECK(ranges::pop_heap(ia, ia) == ia);
    delete [] ia;
}

void test_3(int N)
{
    int* ia = new int [N];
    for (int i = 0; i < N; ++i)
        ia[i] = i;
    std::random_shuffle(ia, ia+N);
    std::make_heap(ia, ia+N);
    for (int i = N; i > 0; --i)
    {
        CHECK(ranges::pop_heap(::as_lvalue(ranges::make_range(ia, ia+i))) == ia+i);
        CHECK(std::is_heap(ia, ia+i-1));
    }
    std::random_shuffle(ia, ia+N);
    std::make_heap(ia, ia+N);
    for (int i = N; i > 0; --i)
    {
        CHECK(ranges::pop_heap(ranges::make_range(ia, ia+i)).get_unsafe() == ia+i);
        CHECK(std::is_heap(ia, ia+i-1));
    }
    CHECK(ranges::pop_heap(ia, ia) == ia);
    delete [] ia;
}

void test_4(int N)
{
    int* ia = new int [N];
    for (int i = 0; i < N; ++i)
        ia[i] = i;
    std::random_shuffle(ia, ia+N);
    std::make_heap(ia, ia+N);
    for (int i = N; i > 0; --i)
    {
        CHECK(ranges::pop_heap(::as_lvalue(ranges::make_range(ia, sentinel<int*>(ia+i)))) == ia+i);
        CHECK(std::is_heap(ia, ia+i-1));
    }
    std::random_shuffle(ia, ia+N);
    std::make_heap(ia, ia+N);
    for (int i = N; i > 0; --i)
    {
        CHECK(ranges::pop_heap(ranges::make_range(ia, sentinel<int*>(ia+i))).get_unsafe() == ia+i);
        CHECK(std::is_heap(ia, ia+i-1));
    }
    CHECK(ranges::pop_heap(ia, ia) == ia);
    delete [] ia;
}

void test_5(int N)
{
    int* ia = new int [N];
    for (int i = 0; i < N; ++i)
        ia[i] = i;
    std::random_shuffle(ia, ia+N);
    std::make_heap(ia, ia+N, std::greater<int>());
    for (int i = N; i > 0; --i)
    {
        CHECK(ranges::pop_heap(ia, ia+i, std::greater<int>()) == ia+i);
        CHECK(std::is_heap(ia, ia+i-1, std::greater<int>()));
    }
    CHECK(ranges::pop_heap(ia, ia, std::greater<int>()) == ia);
    delete [] ia;
}

void test_6(int N)
{
    int* ia = new int [N];
    for (int i = 0; i < N; ++i)
        ia[i] = i;
    std::random_shuffle(ia, ia+N);
    std::make_heap(ia, ia+N, std::greater<int>());
    for (int i = N; i > 0; --i)
    {
        CHECK(ranges::pop_heap(ia, sentinel<int*>(ia+i), std::greater<int>()) == ia+i);
        CHECK(std::is_heap(ia, ia+i-1, std::greater<int>()));
    }
    CHECK(ranges::pop_heap(ia, sentinel<int*>(ia), std::greater<int>()) == ia);
    delete [] ia;
}

void test_7(int N)
{
    int* ia = new int [N];
    for (int i = 0; i < N; ++i)
        ia[i] = i;
    std::random_shuffle(ia, ia+N);
    std::make_heap(ia, ia+N, std::greater<int>());
    for (int i = N; i > 0; --i)
    {
        CHECK(ranges::pop_heap(::as_lvalue(ranges::make_range(ia, ia+i)), std::greater<int>()) == ia+i);
        CHECK(std::is_heap(ia, ia+i-1, std::greater<int>()));
    }
    std::random_shuffle(ia, ia+N);
    std::make_heap(ia, ia+N, std::greater<int>());
    for (int i = N; i > 0; --i)
    {
        CHECK(ranges::pop_heap(ranges::make_range(ia, ia+i), std::greater<int>()).get_unsafe() == ia+i);
        CHECK(std::is_heap(ia, ia+i-1, std::greater<int>()));
    }
    CHECK(ranges::pop_heap(ia, ia, std::greater<int>()) == ia);
    delete [] ia;
}

void test_8(int N)
{
    int* ia = new int [N];
    for (int i = 0; i < N; ++i)
        ia[i] = i;
    std::random_shuffle(ia, ia+N);
    std::make_heap(ia, ia+N, std::greater<int>());
    for (int i = N; i > 0; --i)
    {
        CHECK(ranges::pop_heap(::as_lvalue(ranges::make_range(ia, sentinel<int*>(ia+i))), std::greater<int>()) == ia+i);
        CHECK(std::is_heap(ia, ia+i-1, std::greater<int>()));
    }
    std::random_shuffle(ia, ia+N);
    std::make_heap(ia, ia+N, std::greater<int>());
    for (int i = N; i > 0; --i)
    {
        CHECK(ranges::pop_heap(ranges::make_range(ia, sentinel<int*>(ia+i)), std::greater<int>()).get_unsafe() == ia+i);
        CHECK(std::is_heap(ia, ia+i-1, std::greater<int>()));
    }
    CHECK(ranges::pop_heap(ia, sentinel<int*>(ia), std::greater<int>()) == ia);
    delete [] ia;
}

struct indirect_less
{
    template <class P>
    bool operator()(const P& x, const P& y)
        {return *x < *y;}
};

void test_9(int N)
{
    std::unique_ptr<int>* ia = new std::unique_ptr<int> [N];
    for (int i = 0; i < N; ++i)
        ia[i].reset(new int(i));
    std::random_shuffle(ia, ia+N);
    std::make_heap(ia, ia+N, indirect_less());
    for (int i = N; i > 0; --i)
    {
        CHECK(ranges::pop_heap(ia, ia+i, indirect_less()) == ia+i);
        CHECK(std::is_heap(ia, ia+i-1, indirect_less()));
    }
    delete [] ia;
}

template<typename T>
struct construct
{
    template<typename ...Us>
    T operator()(Us &&... us) const
    {
        return T{((Us &&)us)...};
    }
};

struct S
{
    int i;
};

void test_10(int N)
{
    int* ia = new int [N];
    S* ib = new S [N];
    for (int i = 0; i < N; ++i)
        ia[i] = i;
    std::random_shuffle(ia, ia+N);
    std::make_heap(ia, ia+N);
    std::transform(ia, ia+N, ib, construct<S>());
    for (int i = N; i > 0; --i)
    {
        CHECK(ranges::pop_heap(ib, ib+i, std::less<int>(), &S::i) == ib+i);
        std::transform(ib, ib+i, ia, std::mem_fn(&S::i));
        CHECK(std::is_heap(ia, ia+i-1));
    }
    CHECK(ranges::pop_heap(ib, ib, std::less<int>(), &S::i) == ib);
    delete [] ia;
    delete [] ib;
}

#ifdef RANGES_CXX_GREATER_THAN_11
RANGES_RELAXED_CONSTEXPR bool test_constexpr() {
    using namespace ranges;
    bool r = true;
    constexpr int N = 100;
    array<int, N> ia{{0}};
    for (int i = 0; i < N; ++i)
        ia[i] = i;
    make_heap(ia);
    for (int i = N; i > 0 ; --i)
    {
        if (pop_heap(make_range(begin(ia), begin(ia)+i), std::less<int>()).get_unsafe() != begin(ia) + i) {
            r = false;
        }
        if (!is_heap(begin(ia), begin(ia) + i - 1)) {
            r = false;
        }
    }
    if (pop_heap(make_range(begin(ia), begin(ia)), std::less<int>()).get_unsafe() != begin(ia)) {
        r = false;
    }
    return r;
}
#endif

int main()
{
    test_1(1000);
    test_2(1000);
    test_3(1000);
    test_4(1000);
    test_5(1000);
    test_6(1000);
    test_7(1000);
    test_8(1000);
    test_9(1000);
    test_10(1000);

#ifdef RANGES_CXX_GREATER_THAN_11
    {
        static_assert(test_constexpr(), "");
    }
#endif

    return test_result();
}
