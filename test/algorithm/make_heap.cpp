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

#include <memory>
#include <random>
#include <algorithm>
#include <functional>
#include <range/v3/core.hpp>
#include <range/v3/algorithm/heap_algorithm.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"
#include "../test_iterators.hpp"

RANGES_DIAGNOSTIC_IGNORE_GLOBAL_CONSTRUCTORS
RANGES_DIAGNOSTIC_IGNORE_SIGN_CONVERSION

namespace
{
    std::mt19937 gen;

    void test_1(int N)
    {
        int* ia = new int [N];
        for (int i = 0; i < N; ++i)
            ia[i] = i;
        std::shuffle(ia, ia+N, gen);
        CHECK(ranges::make_heap(ia, ia+N) == ia+N);
        CHECK(std::is_heap(ia, ia+N));
        delete [] ia;
    }

    void test_2(int N)
    {
        int* ia = new int [N];
        for (int i = 0; i < N; ++i)
            ia[i] = i;
        std::shuffle(ia, ia+N, gen);
        CHECK(ranges::make_heap(ia, sentinel<int*>(ia+N)) == ia+N);
        CHECK(std::is_heap(ia, ia+N));
        delete [] ia;
    }

    void test_3(int N)
    {
        int* ia = new int [N];
        for (int i = 0; i < N; ++i)
            ia[i] = i;
        std::shuffle(ia, ia+N, gen);
        CHECK(ranges::make_heap(::as_lvalue(ranges::make_iterator_range(ia, ia+N))) == ia+N);
        CHECK(std::is_heap(ia, ia+N));

        std::shuffle(ia, ia+N, gen);
        CHECK(ranges::make_heap(ranges::make_iterator_range(ia, ia+N)).get_unsafe() == ia+N);
        CHECK(std::is_heap(ia, ia+N));

        delete [] ia;
    }

    void test_4(int N)
    {
        int* ia = new int [N];
        for (int i = 0; i < N; ++i)
            ia[i] = i;
        std::shuffle(ia, ia+N, gen);
        CHECK(ranges::make_heap(::as_lvalue(ranges::make_iterator_range(ia, sentinel<int*>(ia+N)))) == ia+N);
        CHECK(std::is_heap(ia, ia+N));

        std::shuffle(ia, ia+N, gen);
        CHECK(ranges::make_heap(ranges::make_iterator_range(ia, sentinel<int*>(ia+N))).get_unsafe() == ia+N);
        CHECK(std::is_heap(ia, ia+N));

        delete [] ia;
    }

    void test_5(int N)
    {
        int* ia = new int [N];
        for (int i = 0; i < N; ++i)
            ia[i] = i;
        std::shuffle(ia, ia+N, gen);
        CHECK(ranges::make_heap(ia, ia+N, std::greater<int>()) == ia+N);
        CHECK(std::is_heap(ia, ia+N, std::greater<int>()));
        delete [] ia;
    }

    void test_6(int N)
    {
        int* ia = new int [N];
        for (int i = 0; i < N; ++i)
            ia[i] = i;
        std::shuffle(ia, ia+N, gen);
        CHECK(ranges::make_heap(ia, sentinel<int*>(ia+N), std::greater<int>()) == ia+N);
        CHECK(std::is_heap(ia, ia+N, std::greater<int>()));
        delete [] ia;
    }

    void test_7(int N)
    {
        int* ia = new int [N];
        for (int i = 0; i < N; ++i)
            ia[i] = i;
        std::shuffle(ia, ia+N, gen);
        CHECK(ranges::make_heap(::as_lvalue(ranges::make_iterator_range(ia, ia+N)), std::greater<int>()) == ia+N);
        CHECK(std::is_heap(ia, ia+N, std::greater<int>()));

        std::shuffle(ia, ia+N, gen);
        CHECK(ranges::make_heap(ranges::make_iterator_range(ia, ia+N), std::greater<int>()).get_unsafe() == ia+N);
        CHECK(std::is_heap(ia, ia+N, std::greater<int>()));

        delete [] ia;
    }

    void test_8(int N)
    {
        int* ia = new int [N];
        for (int i = 0; i < N; ++i)
            ia[i] = i;
        std::shuffle(ia, ia+N, gen);
        CHECK(ranges::make_heap(::as_lvalue(ranges::make_iterator_range(ia, sentinel<int*>(ia+N))), std::greater<int>()) == ia+N);
        CHECK(std::is_heap(ia, ia+N, std::greater<int>()));

        std::shuffle(ia, ia+N, gen);
        CHECK(ranges::make_heap(ranges::make_iterator_range(ia, sentinel<int*>(ia+N)), std::greater<int>()).get_unsafe() == ia+N);
        CHECK(std::is_heap(ia, ia+N, std::greater<int>()));

        delete [] ia;
    }

    struct indirect_less
    {
        template<class P>
        bool operator()(const P& x, const P& y)
            {return *x < *y;}
    };

    void test_9(int N)
    {
        std::unique_ptr<int>* ia = new std::unique_ptr<int> [N];
        for (int i = 0; i < N; ++i)
            ia[i].reset(new int(i));
        std::shuffle(ia, ia+N, gen);
        CHECK(ranges::make_heap(ia, ia+N, indirect_less()) == ia+N);
        CHECK(std::is_heap(ia, ia+N, indirect_less()));
        delete [] ia;
    }

    struct S
    {
        int i;
    };

    void test_10(int N)
    {
        int* ia = new int [N];
        S* ib = new S [N];
        for (int i = 0; i < N; ++i)
            ib[i].i = i;
        std::shuffle(ia, ia+N, gen);
        CHECK(ranges::make_heap(ib, ib+N, std::less<int>(), &S::i) == ib+N);
        std::transform(ib, ib+N, ia, std::mem_fn(&S::i));
        CHECK(std::is_heap(ia, ia+N));
        delete [] ia;
        delete [] ib;
    }

    void test(int N)
    {
        test_1(N);
        test_2(N);
        test_3(N);
        test_4(N);
        test_5(N);
        test_6(N);
        test_7(N);
        test_8(N);
    }
}

int main()
{
    test(0);
    test(1);
    test(2);
    test(3);
    test(10);
    test(1000);
    test_9(1000);
    test_10(1000);

    return test_result();
}
