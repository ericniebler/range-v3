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

#include <memory>
#include <algorithm>
#include <range/v3/core.hpp>
#include <range/v3/algorithm/move.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"
#include "../test_iterators.hpp"

template<typename InIter, typename OutIter, typename Sent = InIter>
void
test()
{
    {
        const int N = 1000;
        int ia[N];
        for(int i = 0; i < N; ++i)
            ia[i] = i;
        int ib[N] = {0};

        std::pair<InIter, OutIter> r = ranges::move(InIter(ia), Sent(ia+N), OutIter(ib));
        CHECK(base(r.first) == ia+N);
        CHECK(base(r.second) == ib+N);
        for(int i = 0; i < N; ++i)
            CHECK(ia[i] == ib[i]);
    }

    {
        const int N = 1000;
        int ia[N];
        for(int i = 0; i < N; ++i)
            ia[i] = i;
        int ib[N] = {0};

        std::pair<InIter, OutIter> r = ranges::move(as_lvalue(ranges::make_range(InIter(ia), Sent(ia+N))), OutIter(ib));
        CHECK(base(r.first) == ia+N);
        CHECK(base(r.second) == ib+N);
        for(int i = 0; i < N; ++i)
            CHECK(ia[i] == ib[i]);
    }
}

#ifdef RANGES_CXX_GREATER_THAN_11
template<typename InIter, typename OutIter, typename Sent = InIter>
RANGES_CXX14_CONSTEXPR
bool test_constexpr()
{
    {
        constexpr int N = 1000;
        int ia[N]{1};
        for(int i = 0; i < N; ++i)
            ia[i] = i;
        int ib[N] = {0};

        std::pair<InIter, OutIter> r = ranges::move(InIter(ia), Sent(ia+N), OutIter(ib));
        if(base(r.first) != ia+N) { return false; }
        if(base(r.second) != ib+N) { return false; }
        for(int i = 0; i < N; ++i)
            if(ia[i] != ib[i]) { return false; }
    }

    {
        constexpr int N = 1000;
        int ia[N]{1};
        for(int i = 0; i < N; ++i)
            ia[i] = i;
        int ib[N] = {0};

        std::pair<InIter, OutIter> r = ranges::move(as_lvalue(ranges::make_range(InIter(ia), Sent(ia+N))), OutIter(ib));
        if(base(r.first) != ia+N) { return false; }
        if(base(r.second) != ib+N) { return false; }
        for(int i = 0; i < N; ++i)
            if(ia[i] != ib[i]) { return false; }
    }

    return true;
}
#endif

struct S
{
    std::unique_ptr<int> p;
};

template<typename InIter, typename OutIter, typename Sent = InIter>
void
test1()
{
    {
        const int N = 100;
        std::unique_ptr<int> ia[N];
        for(int i = 0; i < N; ++i)
            ia[i].reset(new int(i));
        std::unique_ptr<int> ib[N];

        std::pair<InIter, OutIter> r = ranges::move(InIter(ia), Sent(ia+N), OutIter(ib));
        CHECK(base(r.first) == ia+N);
        CHECK(base(r.second) == ib+N);
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

        std::pair<InIter, OutIter> r = ranges::move(as_lvalue(ranges::make_range(InIter(ia), Sent(ia+N))), OutIter(ib));
        CHECK(base(r.first) == ia+N);
        CHECK(base(r.second) == ib+N);
        for(int i = 0; i < N; ++i)
        {
            CHECK(ia[i].get() == nullptr);
            CHECK(*ib[i] == i);
        }

        ranges::move(ib, ib+N, ia);

        auto r2 = ranges::move(ranges::make_range(InIter(ia), Sent(ia+N)), OutIter(ib));
        CHECK(base(r2.first.get_unsafe()) == ia+N);
        CHECK(base(r2.second) == ib+N);
        for(int i = 0; i < N; ++i)
        {
            CHECK(ia[i].get() == nullptr);
            CHECK(*ib[i] == i);
        }
    }
}

int main()
{
    test<input_iterator<const int*>, output_iterator<int*> >();
    test<input_iterator<const int*>, input_iterator<int*> >();
    test<input_iterator<const int*>, forward_iterator<int*> >();
    test<input_iterator<const int*>, bidirectional_iterator<int*> >();
    test<input_iterator<const int*>, random_access_iterator<int*> >();
    test<input_iterator<const int*>, int*>();

    test<forward_iterator<const int*>, output_iterator<int*> >();
    test<forward_iterator<const int*>, input_iterator<int*> >();
    test<forward_iterator<const int*>, forward_iterator<int*> >();
    test<forward_iterator<const int*>, bidirectional_iterator<int*> >();
    test<forward_iterator<const int*>, random_access_iterator<int*> >();
    test<forward_iterator<const int*>, int*>();

    test<bidirectional_iterator<const int*>, output_iterator<int*> >();
    test<bidirectional_iterator<const int*>, input_iterator<int*> >();
    test<bidirectional_iterator<const int*>, forward_iterator<int*> >();
    test<bidirectional_iterator<const int*>, bidirectional_iterator<int*> >();
    test<bidirectional_iterator<const int*>, random_access_iterator<int*> >();
    test<bidirectional_iterator<const int*>, int*>();

    test<random_access_iterator<const int*>, output_iterator<int*> >();
    test<random_access_iterator<const int*>, input_iterator<int*> >();
    test<random_access_iterator<const int*>, forward_iterator<int*> >();
    test<random_access_iterator<const int*>, bidirectional_iterator<int*> >();
    test<random_access_iterator<const int*>, random_access_iterator<int*> >();
    test<random_access_iterator<const int*>, int*>();

    test<const int*, output_iterator<int*> >();
    test<const int*, input_iterator<int*> >();
    test<const int*, forward_iterator<int*> >();
    test<const int*, bidirectional_iterator<int*> >();
    test<const int*, random_access_iterator<int*> >();
    test<const int*, int*>();

    test<input_iterator<const int*>, output_iterator<int*>, sentinel<const int*>>();
    test<input_iterator<const int*>, input_iterator<int*>, sentinel<const int*> >();
    test<input_iterator<const int*>, forward_iterator<int*>, sentinel<const int*> >();
    test<input_iterator<const int*>, bidirectional_iterator<int*>, sentinel<const int*> >();
    test<input_iterator<const int*>, random_access_iterator<int*>, sentinel<const int*> >();

    test<forward_iterator<const int*>, output_iterator<int*>, sentinel<const int*> >();
    test<forward_iterator<const int*>, input_iterator<int*>, sentinel<const int*> >();
    test<forward_iterator<const int*>, forward_iterator<int*>, sentinel<const int*> >();
    test<forward_iterator<const int*>, bidirectional_iterator<int*>, sentinel<const int*> >();
    test<forward_iterator<const int*>, random_access_iterator<int*>, sentinel<const int*> >();

    test<bidirectional_iterator<const int*>, output_iterator<int*>, sentinel<const int*> >();
    test<bidirectional_iterator<const int*>, input_iterator<int*>, sentinel<const int*> >();
    test<bidirectional_iterator<const int*>, forward_iterator<int*>, sentinel<const int*> >();
    test<bidirectional_iterator<const int*>, bidirectional_iterator<int*>, sentinel<const int*> >();
    test<bidirectional_iterator<const int*>, random_access_iterator<int*>, sentinel<const int*> >();

    test<random_access_iterator<const int*>, output_iterator<int*>, sentinel<const int*> >();
    test<random_access_iterator<const int*>, input_iterator<int*>, sentinel<const int*> >();
    test<random_access_iterator<const int*>, forward_iterator<int*>, sentinel<const int*> >();
    test<random_access_iterator<const int*>, bidirectional_iterator<int*>, sentinel<const int*> >();
    test<random_access_iterator<const int*>, random_access_iterator<int*>, sentinel<const int*> >();

    test1<input_iterator<std::unique_ptr<int>*>, output_iterator<std::unique_ptr<int>*> >();
    test1<input_iterator<std::unique_ptr<int>*>, input_iterator<std::unique_ptr<int>*> >();
    test1<input_iterator<std::unique_ptr<int>*>, forward_iterator<std::unique_ptr<int>*> >();
    test1<input_iterator<std::unique_ptr<int>*>, bidirectional_iterator<std::unique_ptr<int>*> >();
    test1<input_iterator<std::unique_ptr<int>*>, random_access_iterator<std::unique_ptr<int>*> >();
    test1<input_iterator<std::unique_ptr<int>*>, std::unique_ptr<int>*>();

    test1<forward_iterator<std::unique_ptr<int>*>, output_iterator<std::unique_ptr<int>*> >();
    test1<forward_iterator<std::unique_ptr<int>*>, input_iterator<std::unique_ptr<int>*> >();
    test1<forward_iterator<std::unique_ptr<int>*>, forward_iterator<std::unique_ptr<int>*> >();
    test1<forward_iterator<std::unique_ptr<int>*>, bidirectional_iterator<std::unique_ptr<int>*> >();
    test1<forward_iterator<std::unique_ptr<int>*>, random_access_iterator<std::unique_ptr<int>*> >();
    test1<forward_iterator<std::unique_ptr<int>*>, std::unique_ptr<int>*>();

    test1<bidirectional_iterator<std::unique_ptr<int>*>, output_iterator<std::unique_ptr<int>*> >();
    test1<bidirectional_iterator<std::unique_ptr<int>*>, input_iterator<std::unique_ptr<int>*> >();
    test1<bidirectional_iterator<std::unique_ptr<int>*>, forward_iterator<std::unique_ptr<int>*> >();
    test1<bidirectional_iterator<std::unique_ptr<int>*>, bidirectional_iterator<std::unique_ptr<int>*> >();
    test1<bidirectional_iterator<std::unique_ptr<int>*>, random_access_iterator<std::unique_ptr<int>*> >();
    test1<bidirectional_iterator<std::unique_ptr<int>*>, std::unique_ptr<int>*>();

    test1<random_access_iterator<std::unique_ptr<int>*>, output_iterator<std::unique_ptr<int>*> >();
    test1<random_access_iterator<std::unique_ptr<int>*>, input_iterator<std::unique_ptr<int>*> >();
    test1<random_access_iterator<std::unique_ptr<int>*>, forward_iterator<std::unique_ptr<int>*> >();
    test1<random_access_iterator<std::unique_ptr<int>*>, bidirectional_iterator<std::unique_ptr<int>*> >();
    test1<random_access_iterator<std::unique_ptr<int>*>, random_access_iterator<std::unique_ptr<int>*> >();
    test1<random_access_iterator<std::unique_ptr<int>*>, std::unique_ptr<int>*>();

    test1<std::unique_ptr<int>*, output_iterator<std::unique_ptr<int>*> >();
    test1<std::unique_ptr<int>*, input_iterator<std::unique_ptr<int>*> >();
    test1<std::unique_ptr<int>*, forward_iterator<std::unique_ptr<int>*> >();
    test1<std::unique_ptr<int>*, bidirectional_iterator<std::unique_ptr<int>*> >();
    test1<std::unique_ptr<int>*, random_access_iterator<std::unique_ptr<int>*> >();
    test1<std::unique_ptr<int>*, std::unique_ptr<int>*>();

    test1<input_iterator<std::unique_ptr<int>*>, output_iterator<std::unique_ptr<int>*>, sentinel<std::unique_ptr<int>*> >();
    test1<input_iterator<std::unique_ptr<int>*>, input_iterator<std::unique_ptr<int>*>, sentinel<std::unique_ptr<int>*> >();
    test1<input_iterator<std::unique_ptr<int>*>, forward_iterator<std::unique_ptr<int>*>, sentinel<std::unique_ptr<int>*> >();
    test1<input_iterator<std::unique_ptr<int>*>, bidirectional_iterator<std::unique_ptr<int>*>, sentinel<std::unique_ptr<int>*> >();
    test1<input_iterator<std::unique_ptr<int>*>, random_access_iterator<std::unique_ptr<int>*>, sentinel<std::unique_ptr<int>*> >();
    test1<input_iterator<std::unique_ptr<int>*>, std::unique_ptr<int>*>();

    test1<forward_iterator<std::unique_ptr<int>*>, output_iterator<std::unique_ptr<int>*>, sentinel<std::unique_ptr<int>*> >();
    test1<forward_iterator<std::unique_ptr<int>*>, input_iterator<std::unique_ptr<int>*>, sentinel<std::unique_ptr<int>*> >();
    test1<forward_iterator<std::unique_ptr<int>*>, forward_iterator<std::unique_ptr<int>*>, sentinel<std::unique_ptr<int>*> >();
    test1<forward_iterator<std::unique_ptr<int>*>, bidirectional_iterator<std::unique_ptr<int>*>, sentinel<std::unique_ptr<int>*> >();
    test1<forward_iterator<std::unique_ptr<int>*>, random_access_iterator<std::unique_ptr<int>*>, sentinel<std::unique_ptr<int>*> >();
    test1<forward_iterator<std::unique_ptr<int>*>, std::unique_ptr<int>*>();

    test1<bidirectional_iterator<std::unique_ptr<int>*>, output_iterator<std::unique_ptr<int>*>, sentinel<std::unique_ptr<int>*> >();
    test1<bidirectional_iterator<std::unique_ptr<int>*>, input_iterator<std::unique_ptr<int>*>, sentinel<std::unique_ptr<int>*> >();
    test1<bidirectional_iterator<std::unique_ptr<int>*>, forward_iterator<std::unique_ptr<int>*>, sentinel<std::unique_ptr<int>*> >();
    test1<bidirectional_iterator<std::unique_ptr<int>*>, bidirectional_iterator<std::unique_ptr<int>*>, sentinel<std::unique_ptr<int>*> >();
    test1<bidirectional_iterator<std::unique_ptr<int>*>, random_access_iterator<std::unique_ptr<int>*>, sentinel<std::unique_ptr<int>*> >();
    test1<bidirectional_iterator<std::unique_ptr<int>*>, std::unique_ptr<int>*>();

    test1<random_access_iterator<std::unique_ptr<int>*>, output_iterator<std::unique_ptr<int>*>, sentinel<std::unique_ptr<int>*> >();
    test1<random_access_iterator<std::unique_ptr<int>*>, input_iterator<std::unique_ptr<int>*>, sentinel<std::unique_ptr<int>*> >();
    test1<random_access_iterator<std::unique_ptr<int>*>, forward_iterator<std::unique_ptr<int>*>, sentinel<std::unique_ptr<int>*> >();
    test1<random_access_iterator<std::unique_ptr<int>*>, bidirectional_iterator<std::unique_ptr<int>*>, sentinel<std::unique_ptr<int>*> >();
    test1<random_access_iterator<std::unique_ptr<int>*>, random_access_iterator<std::unique_ptr<int>*>, sentinel<std::unique_ptr<int>*> >();

#ifdef RANGES_CXX_GREATER_THAN_11
    {
    static_assert(test_constexpr<input_iterator<const int*>, output_iterator<int*> >(), "");
    static_assert(test_constexpr<input_iterator<const int*>, input_iterator<int*> >(), "");
    static_assert(test_constexpr<input_iterator<const int*>, forward_iterator<int*> >(), "");
    static_assert(test_constexpr<input_iterator<const int*>, bidirectional_iterator<int*> >(), "");
    static_assert(test_constexpr<input_iterator<const int*>, random_access_iterator<int*> >(), "");
    static_assert(test_constexpr<input_iterator<const int*>, int*>(), "");

    static_assert(test_constexpr<forward_iterator<const int*>, output_iterator<int*> >(), "");
    static_assert(test_constexpr<forward_iterator<const int*>, input_iterator<int*> >(), "");
    static_assert(test_constexpr<forward_iterator<const int*>, forward_iterator<int*> >(), "");
    static_assert(test_constexpr<forward_iterator<const int*>, bidirectional_iterator<int*> >(), "");
    static_assert(test_constexpr<forward_iterator<const int*>, random_access_iterator<int*> >(), "");
    static_assert(test_constexpr<forward_iterator<const int*>, int*>(), "");

    static_assert(test_constexpr<bidirectional_iterator<const int*>, output_iterator<int*> >(), "");
    static_assert(test_constexpr<bidirectional_iterator<const int*>, input_iterator<int*> >(), "");
    static_assert(test_constexpr<bidirectional_iterator<const int*>, forward_iterator<int*> >(), "");
    static_assert(test_constexpr<bidirectional_iterator<const int*>, bidirectional_iterator<int*> >(), "");
    static_assert(test_constexpr<bidirectional_iterator<const int*>, random_access_iterator<int*> >(), "");
    static_assert(test_constexpr<bidirectional_iterator<const int*>, int*>(), "");

    static_assert(test_constexpr<random_access_iterator<const int*>, output_iterator<int*> >(), "");
    static_assert(test_constexpr<random_access_iterator<const int*>, input_iterator<int*> >(), "");
    static_assert(test_constexpr<random_access_iterator<const int*>, forward_iterator<int*> >(), "");
    static_assert(test_constexpr<random_access_iterator<const int*>, bidirectional_iterator<int*> >(), "");
    static_assert(test_constexpr<random_access_iterator<const int*>, random_access_iterator<int*> >(), "");
    static_assert(test_constexpr<random_access_iterator<const int*>, int*>(), "");

    static_assert(test_constexpr<const int*, output_iterator<int*> >(), "");
    static_assert(test_constexpr<const int*, input_iterator<int*> >(), "");
    static_assert(test_constexpr<const int*, forward_iterator<int*> >(), "");
    static_assert(test_constexpr<const int*, bidirectional_iterator<int*> >(), "");
    static_assert(test_constexpr<const int*, random_access_iterator<int*> >(), "");
    static_assert(test_constexpr<const int*, int*>(), "");

    static_assert(test_constexpr<input_iterator<const int*>, output_iterator<int*>, sentinel<const int*>>(), "");
    static_assert(test_constexpr<input_iterator<const int*>, input_iterator<int*>, sentinel<const int*> >(), "");
    static_assert(test_constexpr<input_iterator<const int*>, forward_iterator<int*>, sentinel<const int*> >(), "");
    static_assert(test_constexpr<input_iterator<const int*>, bidirectional_iterator<int*>, sentinel<const int*> >(), "");
    static_assert(test_constexpr<input_iterator<const int*>, random_access_iterator<int*>, sentinel<const int*> >(), "");

    static_assert(test_constexpr<forward_iterator<const int*>, output_iterator<int*>, sentinel<const int*> >(), "");
    static_assert(test_constexpr<forward_iterator<const int*>, input_iterator<int*>, sentinel<const int*> >(), "");
    static_assert(test_constexpr<forward_iterator<const int*>, forward_iterator<int*>, sentinel<const int*> >(), "");
    static_assert(test_constexpr<forward_iterator<const int*>, bidirectional_iterator<int*>, sentinel<const int*> >(), "");
    static_assert(test_constexpr<forward_iterator<const int*>, random_access_iterator<int*>, sentinel<const int*> >(), "");

    static_assert(test_constexpr<bidirectional_iterator<const int*>, output_iterator<int*>, sentinel<const int*> >(), "");
    static_assert(test_constexpr<bidirectional_iterator<const int*>, input_iterator<int*>, sentinel<const int*> >(), "");
    static_assert(test_constexpr<bidirectional_iterator<const int*>, forward_iterator<int*>, sentinel<const int*> >(), "");
    static_assert(test_constexpr<bidirectional_iterator<const int*>, bidirectional_iterator<int*>, sentinel<const int*> >(), "");
    static_assert(test_constexpr<bidirectional_iterator<const int*>, random_access_iterator<int*>, sentinel<const int*> >(), "");

    static_assert(test_constexpr<random_access_iterator<const int*>, output_iterator<int*>, sentinel<const int*> >(), "");
    static_assert(test_constexpr<random_access_iterator<const int*>, input_iterator<int*>, sentinel<const int*> >(), "");
    static_assert(test_constexpr<random_access_iterator<const int*>, forward_iterator<int*>, sentinel<const int*> >(), "");
    static_assert(test_constexpr<random_access_iterator<const int*>, bidirectional_iterator<int*>, sentinel<const int*> >(), "");
    static_assert(test_constexpr<random_access_iterator<const int*>, random_access_iterator<int*>, sentinel<const int*> >(), "");
    }
#endif

    return test_result();
}
