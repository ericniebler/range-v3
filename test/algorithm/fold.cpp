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

//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include <functional>

#include <range/v3/algorithm/fold.hpp>
#include <range/v3/algorithm/min.hpp>
#include <range/v3/core.hpp>

#include "../simple_test.hpp"
#include "../test_iterators.hpp"

template<typename I, bool Sized = false>
using RandomAccess = RandomAccessIterator<I>;

template<typename I, bool Sized = false>
using Identity = I;

template<template<typename, bool> class IterT,
         template<typename, bool> class SentT = IterT>
void test_left()
{
    using ranges::make_subrange;
    {
        double da[] = {0.25, 0.75};
        using Iter = IterT<const double *, false>;
        using Sent = SentT<const double *, false>;

        CHECK(ranges::foldl(Iter(da), Sent(da), 1, std::plus<>()) == 1.0);
        CHECK(ranges::foldl(Iter(da), Sent(da + 2), 1, std::plus<>()) == 2.0);
        CHECK(ranges::foldl(make_subrange(Iter(da), Sent(da)), 1, std::plus<>()) == 1.0);
        CHECK(ranges::foldl(make_subrange(Iter(da), Sent(da + 2)), 1, std::plus<>()) ==
              2.0);
    }

    {
        int ia[] = {1, 2};
        using Iter = IterT<const int *, false>;
        using Sent = SentT<const int *, false>;
        CHECK(ranges::foldl1(Iter(ia), Sent(ia), ranges::min) == ranges::nullopt);
        CHECK(ranges::foldl1(Iter(ia), Sent(ia + 2), ranges::min) ==
              ranges::optional<int>(1));

        CHECK(ranges::foldl1(make_subrange(Iter(ia), Sent(ia)), ranges::min) ==
              ranges::nullopt);
        CHECK(ranges::foldl1(make_subrange(Iter(ia), Sent(ia + 2)), ranges::min) ==
              ranges::optional<int>(1));
    }
}

void test_right()
{
    double ia[] = {0.25, 0.75};
    CHECK(ranges::foldr(ia, ia + 2, 1, std::plus<>()) == 2.0);
    CHECK(ranges::foldr(ia, 1, std::plus<>()) == 2.0);

    // f(0.25, f(0.75, 1))
    CHECK(ranges::foldr(ia, ia + 2, 1, std::minus<>()) == 0.5);
    CHECK(ranges::foldr(ia, 1, std::minus<>()) == 0.5);

    int xs[] = {1, 2, 3};
    auto concat = [](int i, std::string s) { return s + std::to_string(i); };
    CHECK(ranges::foldr(xs, xs + 2, std::string(), concat) == "21");
    CHECK(ranges::foldr(xs, std::string(), concat) == "321");
}

int main()
{
    test_left<InputIterator>();
    test_left<ForwardIterator>();
    test_left<BidirectionalIterator>();
    test_left<RandomAccess>();
    test_left<Identity>();

    test_left<InputIterator, Sentinel>();
    test_left<ForwardIterator, Sentinel>();
    test_left<BidirectionalIterator, Sentinel>();
    test_left<RandomAccess, Sentinel>();

    test_right();

    return ::test_result();
}
