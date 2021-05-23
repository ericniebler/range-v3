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
#include <range/v3/core.hpp>

#include "../simple_test.hpp"
#include "../test_iterators.hpp"

template<class Iter, class Sent = Iter>
void test()
{
    double ia[] = {0.25, 0.75};
    CHECK(ranges::foldl(Iter(ia), Sent(ia), 1, std::plus()) == 1.0);
    CHECK(ranges::foldl(Iter(ia), Sent(ia + 2), 1, std::plus()) == 2.0);

    using ranges::make_subrange;
    CHECK(ranges::foldl(make_subrange(Iter(ia), Sent(ia)), 1, std::plus()) == 1.0);
    CHECK(ranges::foldl(make_subrange(Iter(ia), Sent(ia + 2)), 1, std::plus()) == 2.0);
}

int main()
{
    test<InputIterator<const double *>>();
    test<ForwardIterator<const double *>>();
    test<BidirectionalIterator<const double *>>();
    test<RandomAccessIterator<const double *>>();
    test<const double *>();

    test<InputIterator<const double *>, Sentinel<const double *>>();
    test<ForwardIterator<const double *>, Sentinel<const double *>>();
    test<BidirectionalIterator<const double *>, Sentinel<const double *>>();
    test<RandomAccessIterator<const double *>, Sentinel<const double *>>();

    return ::test_result();
}
