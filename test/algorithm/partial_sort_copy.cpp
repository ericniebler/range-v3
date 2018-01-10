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
#include <vector>
#include <algorithm>
#include <range/v3/core.hpp>
#include <range/v3/algorithm/partial_sort_copy.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"
#include "../test_iterators.hpp"

RANGES_DIAGNOSTIC_IGNORE_GLOBAL_CONSTRUCTORS
RANGES_DIAGNOSTIC_IGNORE_SIGN_CONVERSION

namespace
{
    std::mt19937 gen;

    template<class Iter>
    void
    test_larger_sorts(int N, int M)
    {
        auto partial_sort_copy = ::make_testable_2<true, false>(ranges::partial_sort_copy);
        int* input = new int[N];
        int* output = new int[M];
        for (int i = 0; i < N; ++i)
            input[i] = i;
        std::shuffle(input, input+N, gen);
        partial_sort_copy(Iter(input), Iter(input+N), output, output+M).check([&](int* r)
        {
            int* e = output + std::min(N, M);
            CHECK(r == e);
            int i = 0;
            for (int* x = output; x < e; ++x, ++i)
                CHECK(*x == i);
            std::shuffle(input, input+N, gen);
        });
        partial_sort_copy(Iter(input), Iter(input+N), output, output+M, std::greater<int>()).check([&](int* r)
        {
            int* e = output + std::min(N, M);
            CHECK(r == e);
            int i = N-1;
            for (int* x = output; x < e; ++x, --i)
                CHECK(*x == i);
            std::shuffle(input, input+N, gen);
        });
        delete [] output;
        delete [] input;
    }

    template<class Iter>
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

    template<class Iter>
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
}

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
        std::shuffle(input, input+N, gen);
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
        std::shuffle(input, input+N, gen);
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

    return ::test_result();
}
