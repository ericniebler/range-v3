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
#include <algorithm>
#include <range/v3/core.hpp>
#include <range/v3/algorithm/move_backward.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"
#include "../test_iterators.hpp"

template<typename InIter, typename OutIter>
void
test()
{
    {
        const int N = 1000;
        int ia[N];
        for(int i = 0; i < N; ++i)
            ia[i] = i;
        int ib[N] = {0};

        ranges::move_backward_result<InIter, OutIter> r =
            ranges::move_backward(InIter(ia), InIter(ia+N), OutIter(ib+N));
        CHECK(base(r.in) == ia+N);
        CHECK(base(r.out) == ib);
        for(int i = 0; i < N; ++i)
            CHECK(ia[i] == ib[i]);
    }

    {
        const int N = 1000;
        int ia[N];
        for(int i = 0; i < N; ++i)
            ia[i] = i;
        int ib[N] = {0};

        ranges::move_backward_result<InIter, OutIter> r =
            ranges::move_backward(ranges::make_subrange(InIter(ia), InIter(ia+N)), OutIter(ib+N));
        CHECK(base(r.in) == ia+N);
        CHECK(base(r.out) == ib);
        for(int i = 0; i < N; ++i)
            CHECK(ia[i] == ib[i]);
    }
}

struct S
{
    std::unique_ptr<int> p;
};

template<typename InIter, typename OutIter>
void
test1()
{
    {
        const int N = 100;
        std::unique_ptr<int> ia[N];
        for(int i = 0; i < N; ++i)
            ia[i].reset(new int(i));
        std::unique_ptr<int> ib[N];

        ranges::move_backward_result<InIter, OutIter> r =
            ranges::move_backward(InIter(ia), InIter(ia+N), OutIter(ib+N));
        CHECK(base(r.in) == ia+N);
        CHECK(base(r.out) == ib);
        for(int i = 0; i < N; ++i)
        {
            CHECK(ia[i].get() == nullptr);
            CHECK(*ib[i] == i);
        }
    }

    {
        const int N = 100;
        std::unique_ptr<int> ia[N];
        for(int i = 0; i < N; ++i)
            ia[i].reset(new int(i));
        std::unique_ptr<int> ib[N];

        ranges::move_backward_result<InIter, OutIter> r =
            ranges::move_backward(ranges::make_subrange(InIter(ia), InIter(ia+N)), OutIter(ib+N));
        CHECK(base(r.in) == ia+N);
        CHECK(base(r.out) == ib);
        for(int i = 0; i < N; ++i)
        {
            CHECK(ia[i].get() == nullptr);
            CHECK(*ib[i] == i);
        }

        ranges::move_backward(ib, ib+N, ia+N);

        auto r2 = ranges::move_backward(ranges::make_subrange(InIter(ia), InIter(ia+N)), OutIter(ib+N));
        CHECK(base(r2.in) == ia+N);
        CHECK(base(r2.out) == ib);
        for(int i = 0; i < N; ++i)
        {
            CHECK(ia[i].get() == nullptr);
            CHECK(*ib[i] == i);
        }
    }
}

int main()
{
    test<BidirectionalIterator<const int*>, BidirectionalIterator<int*> >();
    test<BidirectionalIterator<const int*>, RandomAccessIterator<int*> >();
    test<BidirectionalIterator<const int*>, int*>();

    test<RandomAccessIterator<const int*>, BidirectionalIterator<int*> >();
    test<RandomAccessIterator<const int*>, RandomAccessIterator<int*> >();
    test<RandomAccessIterator<const int*>, int*>();

    test<const int*, BidirectionalIterator<int*> >();
    test<const int*, RandomAccessIterator<int*> >();
    test<const int*, int*>();

    test1<BidirectionalIterator<std::unique_ptr<int>*>, BidirectionalIterator<std::unique_ptr<int>*> >();
    test1<BidirectionalIterator<std::unique_ptr<int>*>, RandomAccessIterator<std::unique_ptr<int>*> >();
    test1<BidirectionalIterator<std::unique_ptr<int>*>, std::unique_ptr<int>*>();

    test1<RandomAccessIterator<std::unique_ptr<int>*>, BidirectionalIterator<std::unique_ptr<int>*> >();
    test1<RandomAccessIterator<std::unique_ptr<int>*>, RandomAccessIterator<std::unique_ptr<int>*> >();
    test1<RandomAccessIterator<std::unique_ptr<int>*>, std::unique_ptr<int>*>();

    test1<std::unique_ptr<int>*, BidirectionalIterator<std::unique_ptr<int>*> >();
    test1<std::unique_ptr<int>*, RandomAccessIterator<std::unique_ptr<int>*> >();
    test1<std::unique_ptr<int>*, std::unique_ptr<int>*>();

    return test_result();
}
