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

//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include <cassert>
#include <memory>
#include <vector>
#include <algorithm>
#include <range/v3/core.hpp>
#include <range/v3/algorithm/partial_sort_copy.hpp>
#include "../safe_int_swap.hpp"
#include "../simple_test.hpp"
#include "../test_utils.hpp"
#include "../test_iterators.hpp"

template <class Iter>
void
test_larger_sorts(int N, int M)
{
    auto partial_sort_copy = ::make_testable_2<true, false>(ranges::partial_sort_copy);
    int* input = new int[N];
    int* output = new int[M];
    for (int i = 0; i < N; ++i)
        input[i] = i;
    std::random_shuffle(input, input+N);
    partial_sort_copy(Iter(input), Iter(input+N), output, output+M).check([&](int* r)
    {
        int* e = output + std::min(N, M);
        CHECK(r == e);
        int i = 0;
        for (int* x = output; x < e; ++x, ++i)
            CHECK(*x == i);
        std::random_shuffle(input, input+N);
    });
    partial_sort_copy(Iter(input), Iter(input+N), output, output+M, std::greater<int>()).check([&](int* r)
    {
        int* e = output + std::min(N, M);
        CHECK(r == e);
        int i = N-1;
        for (int* x = output; x < e; ++x, --i)
            CHECK(*x == i);
        std::random_shuffle(input, input+N);
    });
    delete [] output;
    delete [] input;
}

template <class Iter>
void
test_larger_sorts(int N)
{
    test_larger_sorts<Iter>(N, 0);
    test_larger_sorts<Iter>(N, 1);
    test_larger_sorts<Iter>(N, 2);
    test_larger_sorts<Iter>(N, 3);
    test_larger_sorts<Iter>(N, N/2-1);
    test_larger_sorts<Iter>(N, N/2);
    test_larger_sorts<Iter>(N, N/2+1);
    test_larger_sorts<Iter>(N, N-2);
    test_larger_sorts<Iter>(N, N-1);
    test_larger_sorts<Iter>(N, N);
    test_larger_sorts<Iter>(N, N+1000);
}

template <class Iter>
void
test()
{
    test_larger_sorts<Iter>(0, 100);
    test_larger_sorts<Iter>(10);
    test_larger_sorts<Iter>(256);
    test_larger_sorts<Iter>(257);
    test_larger_sorts<Iter>(499);
    test_larger_sorts<Iter>(500);
    test_larger_sorts<Iter>(997);
    test_larger_sorts<Iter>(1000);
    test_larger_sorts<Iter>(1009);
}

struct S
{
    int i;
};

struct U
{
    int i;
    U & operator=(S s)
    {
        i = s.i;
        return *this;
    }
};

#ifdef RANGES_CXX_GREATER_THAN_11
RANGES_CXX14_CONSTEXPR bool test_constexpr()
{
    using namespace ranges;
    safe_int<int> output[9] = {0};
    safe_int<int>* r = partial_sort_copy({5, 3, 4, 1, 8, 2, 6, 7, 0, 9}, output, less{});
    safe_int<int>* e = output + 9;
    if(r != e) { return false; }
    safe_int<int> i = 0;
    for (safe_int<int>* x = output; x < e; ++x, ++i)
        if(*x != i) { return false; }
    return true;
}
#endif

int main()
{
    int i = 0;
    int * r = ranges::partial_sort_copy(&i, &i, &i, &i+5);
    CHECK(r == &i);
    CHECK(i == 0);
    test<input_iterator<const int*> >();
    test<forward_iterator<const int*> >();
    test<bidirectional_iterator<const int*> >();
    test<random_access_iterator<const int*> >();
    test<const int*>();

    // Check projections
    {
        constexpr int N = 256;
        constexpr int M = N/2-1;
        S input[N];
        U output[M];
        for (int i = 0; i < N; ++i)
            input[i].i = i;
        std::random_shuffle(input, input+N);
        U * r = ranges::partial_sort_copy(input, output, std::less<int>(), &S::i, &U::i);
        U* e = output + std::min(N, M);
        CHECK(r == e);
        int i = 0;
        for (U* x = output; x < e; ++x, ++i)
            CHECK(x->i == i);
    }

    // Check rvalue ranges
    {
        constexpr int N = 256;
        constexpr int M = N/2-1;
        S input[N];
        U output[M];
        for (int i = 0; i < N; ++i)
            input[i].i = i;
        std::random_shuffle(input, input+N);
        auto r = ranges::partial_sort_copy(input, ranges::view::all(output), std::less<int>(), &S::i, &U::i);
        U* e = output + std::min(N, M);
        CHECK(r.get_unsafe() == e);
        int i = 0;
        for (U* x = output; x < e; ++x, ++i)
            CHECK(x->i == i);
    }

    // Check initialize_list
    {
        U output[9];
        U * r = ranges::partial_sort_copy(
            {S{5}, S{3}, S{4}, S{1}, S{8}, S{2}, S{6}, S{7}, S{0}, S{9}},
            output, std::less<int>(), &S::i, &U::i);
        U* e = output + 9;
        CHECK(r == e);
        int i = 0;
        for (U* x = output; x < e; ++x, ++i)
            CHECK(x->i == i);
    }

#ifdef RANGES_CXX_GREATER_THAN_11
    {
        static_assert(test_constexpr(), "");
    }
#endif

    return ::test_result();
}
