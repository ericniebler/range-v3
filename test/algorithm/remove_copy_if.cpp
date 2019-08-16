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

#include <functional>
#include <memory>
#include <utility>
#include <vector>
#include <range/v3/core.hpp>
#include <range/v3/algorithm/remove_copy_if.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"
#include "../test_iterators.hpp"

template<class InIter, class OutIter, class Sent = InIter>
void
test_iter()
{
    int ia[] = {0, 1, 2, 3, 4, 2, 3, 4, 2};
    constexpr auto sa = ranges::size(ia);
    int ib[sa];
    ranges::remove_copy_if_result<InIter, OutIter> r = ranges::remove_copy_if(InIter(ia), Sent(ia+sa), OutIter(ib), [](int i){return i == 2;});
    CHECK(base(r.in) == ia + sa);
    CHECK(base(r.out) == ib + sa-3);
    CHECK(ib[0] == 0);
    CHECK(ib[1] == 1);
    CHECK(ib[2] == 3);
    CHECK(ib[3] == 4);
    CHECK(ib[4] == 3);
    CHECK(ib[5] == 4);
}

template<class InIter, class OutIter, class Sent = InIter>
void
test_range()
{
    int ia[] = {0, 1, 2, 3, 4, 2, 3, 4, 2};
    constexpr auto sa = ranges::size(ia);
    int ib[sa];
    ranges::remove_copy_if_result<InIter, OutIter> r = ranges::remove_copy_if(::as_lvalue(ranges::make_subrange(InIter(ia), Sent(ia+sa))), OutIter(ib), [](int i){return i == 2;});
    CHECK(base(r.in) == ia + sa);
    CHECK(base(r.out) == ib + sa-3);
    CHECK(ib[0] == 0);
    CHECK(ib[1] == 1);
    CHECK(ib[2] == 3);
    CHECK(ib[3] == 4);
    CHECK(ib[4] == 3);
    CHECK(ib[5] == 4);
}

template<class InIter, class OutIter, class Sent = InIter>
void
test()
{
    test_iter<InIter, OutIter, Sent>();
    test_range<InIter, OutIter, Sent>();
}

struct S
{
    int i;
};

int main()
{
    test<InputIterator<const int*>, OutputIterator<int*>>();
    test<InputIterator<const int*>, ForwardIterator<int*>>();
    test<InputIterator<const int*>, BidirectionalIterator<int*>>();
    test<InputIterator<const int*>, RandomAccessIterator<int*>>();
    test<InputIterator<const int*>, int*>();

    test<ForwardIterator<const int*>, OutputIterator<int*>>();
    test<ForwardIterator<const int*>, ForwardIterator<int*>>();
    test<ForwardIterator<const int*>, BidirectionalIterator<int*>>();
    test<ForwardIterator<const int*>, RandomAccessIterator<int*>>();
    test<ForwardIterator<const int*>, int*>();

    test<BidirectionalIterator<const int*>, OutputIterator<int*>>();
    test<BidirectionalIterator<const int*>, ForwardIterator<int*>>();
    test<BidirectionalIterator<const int*>, BidirectionalIterator<int*>>();
    test<BidirectionalIterator<const int*>, RandomAccessIterator<int*>>();
    test<BidirectionalIterator<const int*>, int*>();

    test<RandomAccessIterator<const int*>, OutputIterator<int*>>();
    test<RandomAccessIterator<const int*>, ForwardIterator<int*>>();
    test<RandomAccessIterator<const int*>, BidirectionalIterator<int*>>();
    test<RandomAccessIterator<const int*>, RandomAccessIterator<int*>>();
    test<RandomAccessIterator<const int*>, int*>();

    test<const int*, OutputIterator<int*>>();
    test<const int*, ForwardIterator<int*>>();
    test<const int*, BidirectionalIterator<int*>>();
    test<const int*, RandomAccessIterator<int*>>();
    test<const int*, int*>();

    test<InputIterator<const int*>, OutputIterator<int*>, Sentinel<const int*>>();
    test<InputIterator<const int*>, ForwardIterator<int*>, Sentinel<const int*>>();
    test<InputIterator<const int*>, BidirectionalIterator<int*>, Sentinel<const int*>>();
    test<InputIterator<const int*>, RandomAccessIterator<int*>, Sentinel<const int*>>();
    test<InputIterator<const int*>, int*, Sentinel<const int*>>();

    test<ForwardIterator<const int*>, OutputIterator<int*>, Sentinel<const int*>>();
    test<ForwardIterator<const int*>, ForwardIterator<int*>, Sentinel<const int*>>();
    test<ForwardIterator<const int*>, BidirectionalIterator<int*>, Sentinel<const int*>>();
    test<ForwardIterator<const int*>, RandomAccessIterator<int*>, Sentinel<const int*>>();
    test<ForwardIterator<const int*>, int*, Sentinel<const int*>>();

    test<BidirectionalIterator<const int*>, OutputIterator<int*>, Sentinel<const int*>>();
    test<BidirectionalIterator<const int*>, ForwardIterator<int*>, Sentinel<const int*>>();
    test<BidirectionalIterator<const int*>, BidirectionalIterator<int*>, Sentinel<const int*>>();
    test<BidirectionalIterator<const int*>, RandomAccessIterator<int*>, Sentinel<const int*>>();
    test<BidirectionalIterator<const int*>, int*, Sentinel<const int*>>();

    test<RandomAccessIterator<const int*>, OutputIterator<int*>, Sentinel<const int*>>();
    test<RandomAccessIterator<const int*>, ForwardIterator<int*>, Sentinel<const int*>>();
    test<RandomAccessIterator<const int*>, BidirectionalIterator<int*>, Sentinel<const int*>>();
    test<RandomAccessIterator<const int*>, RandomAccessIterator<int*>, Sentinel<const int*>>();
    test<RandomAccessIterator<const int*>, int*, Sentinel<const int*>>();

    // Check projection
    {
        S ia[] = {S{0}, S{1}, S{2}, S{3}, S{4}, S{2}, S{3}, S{4}, S{2}};
        constexpr auto sa = ranges::size(ia);
        S ib[sa];
        ranges::remove_copy_if_result<S*, S*> r = ranges::remove_copy_if(ia, ib, [](int i){return i == 2;}, &S::i);
        CHECK(r.in == ia + sa);
        CHECK(r.out == ib + sa-3);
        CHECK(ib[0].i == 0);
        CHECK(ib[1].i == 1);
        CHECK(ib[2].i == 3);
        CHECK(ib[3].i == 4);
        CHECK(ib[4].i == 3);
        CHECK(ib[5].i == 4);
    }

    // Check rvalue range
    {
        S ia[] = {S{0}, S{1}, S{2}, S{3}, S{4}, S{2}, S{3}, S{4}, S{2}};
        constexpr auto sa = ranges::size(ia);
        S ib[sa] = {};
        auto r0 = ranges::remove_copy_if(std::move(ia), ib, [](int i){return i == 2;}, &S::i);
#ifndef RANGES_WORKAROUND_MSVC_573728
        CHECK(::is_dangling(r0.in));
#endif // RANGES_WORKAROUND_MSVC_573728
        CHECK(r0.out == ib + sa-3);
        CHECK(ib[0].i == 0);
        CHECK(ib[1].i == 1);
        CHECK(ib[2].i == 3);
        CHECK(ib[3].i == 4);
        CHECK(ib[4].i == 3);
        CHECK(ib[5].i == 4);

        std::fill(ranges::begin(ib), ranges::end(ib), S{});
        std::vector<S> vec(ranges::begin(ia), ranges::end(ia));
        auto r1 = ranges::remove_copy_if(std::move(vec), ib, [](int i){return i == 2;}, &S::i);
        CHECK(::is_dangling(r1.in));
        CHECK(r1.out == ib + sa-3);
        CHECK(ib[0].i == 0);
        CHECK(ib[1].i == 1);
        CHECK(ib[2].i == 3);
        CHECK(ib[3].i == 4);
        CHECK(ib[4].i == 3);
        CHECK(ib[5].i == 4);
    }

    return ::test_result();
}
