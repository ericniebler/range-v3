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
//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include <cassert>
#include <algorithm>
#include <range/v3/core.hpp>
#include <range/v3/algorithm/inplace_merge.hpp>
#include <range/v3/algorithm/sort.hpp>
#include <range/v3/algorithm/is_sorted.hpp>
#include <range/v3/utility/array.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"
#include "../test_iterators.hpp"

template <class Iter, typename Sent = Iter>
void
test_one_iter(unsigned N, unsigned M)
{
    assert(M <= N);
    int* ia = new int[N];
    for (unsigned i = 0; i < N; ++i)
        ia[i] = i;
    std::random_shuffle(ia, ia+N);
    std::sort(ia, ia+M);
    std::sort(ia+M, ia+N);
    auto res = ranges::inplace_merge(Iter(ia), Iter(ia+M), Sent(ia+N));
    CHECK(res == Iter(ia+N));
    if(N > 0)
    {
        CHECK(ia[0] == 0);
        CHECK(ia[N-1] == (int)N-1);
        CHECK(std::is_sorted(ia, ia+N));
    }
    delete [] ia;
}

template <class Iter, typename Sent = Iter>
void
test_one_rng(unsigned N, unsigned M)
{
    assert(M <= N);
    int* ia = new int[N];
    for (unsigned i = 0; i < N; ++i)
        ia[i] = i;
    std::random_shuffle(ia, ia+N);
    std::sort(ia, ia+M);
    std::sort(ia+M, ia+N);
    auto res = ranges::inplace_merge(::as_lvalue(ranges::make_range(Iter(ia), Sent(ia+N))), Iter(ia+M));
    CHECK(res == Iter(ia+N));
    if(N > 0)
    {
        CHECK(ia[0] == 0);
        CHECK(ia[N-1] == (int)N-1);
        CHECK(std::is_sorted(ia, ia+N));
    }

    std::random_shuffle(ia, ia+N);
    std::sort(ia, ia+M);
    std::sort(ia+M, ia+N);
    auto res2 = ranges::inplace_merge(ranges::make_range(Iter(ia), Sent(ia+N)), Iter(ia+M));
    CHECK(res2.get_unsafe() == Iter(ia+N));
    if(N > 0)
    {
        CHECK(ia[0] == 0);
        CHECK(ia[N-1] == (int)N-1);
        CHECK(std::is_sorted(ia, ia+N));
    }

    delete [] ia;
}

template <class Iter>
void
test_one(unsigned N, unsigned M)
{
    test_one_iter<Iter>(N, M);
    test_one_iter<Iter, typename sentinel_type<Iter>::type>(N, M);
    test_one_rng<Iter>(N, M);
    test_one_rng<Iter, typename sentinel_type<Iter>::type>(N, M);
}

template <class Iter>
void
test(unsigned N)
{
    test_one<Iter>(N, 0);
    test_one<Iter>(N, N/4);
    test_one<Iter>(N, N/2);
    test_one<Iter>(N, 3*N/4);
    test_one<Iter>(N, N);
}

template <class Iter>
void
test()
{
    test_one<Iter>(0, 0);
    test_one<Iter>(1, 0);
    test_one<Iter>(1, 1);
    test_one<Iter>(2, 0);
    test_one<Iter>(2, 1);
    test_one<Iter>(2, 2);
    test_one<Iter>(3, 0);
    test_one<Iter>(3, 1);
    test_one<Iter>(3, 2);
    test_one<Iter>(3, 3);
    test<Iter>(4);
    test<Iter>(100);
    test<Iter>(1000);
}


#ifdef RANGES_CXX_GREATER_THAN_11
RANGES_RELAXED_CONSTEXPR bool test_constexpr() {
    using namespace ranges;
    bool r = true;
    constexpr int N = 100;
    constexpr int M = N/2;
    array<int, N> ia{{0}};
    for (int i = 0; i < N; ++i)
        ia[i] = N - 1 - i;
    sort(begin(ia), begin(ia) + M);
    sort(begin(ia) + M, end(ia));
    auto res = detail::inplace_merge_no_buffer(::as_lvalue(make_range(begin(ia), end(ia))), begin(ia) + M);
    if (res != end(ia)) { r = false; }
    if (ia[0] != 0) { r = false; }
    if(ia[N - 1] != N-1) { r = false; }
    if(!is_sorted(ia)) { r = false;}

    auto res2 = detail::inplace_merge_no_buffer(make_range(begin(ia), end(ia)), begin(ia) + M);
    if (res2.get_unsafe() != end(ia)) { r = false; }
    if (ia[0] != 0) { r = false; }
    if(ia[N - 1] != N-1) { r = false; }
    if(!is_sorted(ia)) { r = false;}

    return r;
}
#endif

int main()
{
    // test<forward_iterator<int*> >();
    test<bidirectional_iterator<int*> >();
    test<random_access_iterator<int*> >();
    test<int*>();

#ifdef RANGES_CXX_GREATER_THAN_11
    {
        static_assert(test_constexpr(), "");
    }
#endif

    return ::test_result();
}
