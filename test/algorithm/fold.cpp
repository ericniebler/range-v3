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

RANGES_DIAGNOSTIC_PUSH
RANGES_DIAGNOSTIC_IGNORE_FLOAT_EQUAL

template<class Iter, class Sent = Iter>
void test_left()
{
    double ia[] = {0.25, 0.75};
    CHECK(ranges::foldl(Iter(ia), Sent(ia), 1, std::plus<>()) == 1.0);
    CHECK(ranges::foldl(Iter(ia), Sent(ia + 2), 1, std::plus<>()) == 2.0);

    CHECK(ranges::foldl1(Iter(ia), Sent(ia), ranges::min) == ranges::nullopt);
    CHECK(ranges::foldl1(Iter(ia), Sent(ia + 2), ranges::min) ==
          ranges::optional<double>(0.25));

    using ranges::make_subrange;
    CHECK(ranges::foldl(make_subrange(Iter(ia), Sent(ia)), 1, std::plus<>()) == 1.0);
    CHECK(ranges::foldl(make_subrange(Iter(ia), Sent(ia + 2)), 1, std::plus<>()) == 2.0);
    CHECK(ranges::foldl1(make_subrange(Iter(ia), Sent(ia)), ranges::min) ==
          ranges::nullopt);
    CHECK(ranges::foldl1(make_subrange(Iter(ia), Sent(ia + 2)), ranges::min) ==
          ranges::optional<double>(0.25));
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

RANGES_DIAGNOSTIC_POP

int main()
{
    test_left<InputIterator<const double *>>();
    test_left<ForwardIterator<const double *>>();
    test_left<BidirectionalIterator<const double *>>();
    test_left<RandomAccessIterator<const double *>>();
    test_left<const double *>();

    test_left<InputIterator<const double *>, Sentinel<const double *>>();
    test_left<ForwardIterator<const double *>, Sentinel<const double *>>();
    test_left<BidirectionalIterator<const double *>, Sentinel<const double *>>();
    test_left<RandomAccessIterator<const double *>, Sentinel<const double *>>();

    test_right();

    return ::test_result();
}
