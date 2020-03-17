// Range v3 library
//
//  Copyright Eric Niebler 2014-present
//  Copyright Gonzalo Brito Gadeschi 2014
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

// Implementation based on the code in libc++
//   http://http://libcxx.llvm.org/

#include <vector>

#include <range/v3/algorithm/is_sorted_until.hpp>
#include <range/v3/core.hpp>

#include "../simple_test.hpp"
#include "../test_iterators.hpp"
#include "../test_utils.hpp"

/// Calls the iterator interface of the algorithm
template<class Iter>
struct iter_call
{
    using begin_t = Iter;
    using sentinel_t = typename sentinel_type<Iter>::type;

    template<class B, class E, class... Args>
    auto operator()(B && It, E && e, Args &&... args)
        -> decltype(ranges::is_sorted_until(begin_t{It}, sentinel_t{e},
                                            std::forward<Args>(args)...))
    {
        return ranges::is_sorted_until(
            begin_t{It}, sentinel_t{e}, std::forward<Args>(args)...);
    }
};

/// Calls the range interface of the algorithm
template<class Iter>
struct range_call
{
    using begin_t = Iter;
    using sentinel_t = typename sentinel_type<Iter>::type;

    template<class B, class E, class... Args>
    static auto _impl(B && It, E && e, Args &&... args)
        -> decltype(ranges::is_sorted_until(
            ::as_lvalue(ranges::make_subrange(begin_t{It}, sentinel_t{e})),
            std::forward<Args>(args)...))
    {
        return ranges::is_sorted_until(
            ::as_lvalue(ranges::make_subrange(begin_t{It}, sentinel_t{e})),
            std::forward<Args>(args)...);
    }

    template<class B, class E>
    auto operator()(B && It, E && e) const -> decltype(ranges::is_sorted_until(
        ::as_lvalue(ranges::make_subrange(begin_t{It}, sentinel_t{e}))))
    {
        return range_call::_impl(static_cast<B &&>(It), static_cast<E &&>(e));
    }

    template<class B, class E, class A0>
    auto operator()(B && It, E && e, A0 && a0) const
        -> decltype(ranges::is_sorted_until(::as_lvalue(
            ranges::make_subrange(begin_t{It}, sentinel_t{e})), static_cast<A0 &&>(a0)))
    {
        return range_call::_impl(
            static_cast<B &&>(It), static_cast<E &&>(e), static_cast<A0 &&>(a0));
    }

    template<class B, class E, class A0, class A1>
    auto operator()(B && It, E && e, A0 && a0, A1 && a1) const
        -> decltype(ranges::is_sorted_until(::as_lvalue(ranges::make_subrange(
            begin_t{It}, sentinel_t{e})), static_cast<A0 &&>(a0), static_cast<A1 &&>(a1)))
    {
        return range_call::_impl(
            static_cast<B &&>(It), static_cast<E &&>(e), static_cast<A0 &&>(a0), static_cast<A1 &&>(a1));
    }
};

template<class It, template<class> class FunT>
void test()
{
    using Fun = FunT<It>;

    {
        int a[] = {0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(Fun{}(a, a) == It(a));
        CHECK(Fun{}(a, a + sa) == It(a + sa));
    }

    {
        int a[] = {0, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(Fun{}(a, a + sa) == It(a + sa));
    }
    {
        int a[] = {0, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(Fun{}(a, a + sa) == It(a + sa));
    }
    {
        int a[] = {1, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(Fun{}(a, a + sa) == It(a + 1));
    }
    {
        int a[] = {1, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(Fun{}(a, a + sa) == It(a + sa));
    }

    {
        int a[] = {0, 0, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(Fun{}(a, a + sa) == It(a + sa));
    }
    {
        int a[] = {0, 0, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(Fun{}(a, a + sa) == It(a + sa));
    }
    {
        int a[] = {0, 1, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(Fun{}(a, a + sa) == It(a + 2));
    }
    {
        int a[] = {0, 1, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(Fun{}(a, a + sa) == It(a + sa));
    }
    {
        int a[] = {1, 0, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(Fun{}(a, a + sa) == It(a + 1));
    }
    {
        int a[] = {1, 0, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(Fun{}(a, a + sa) == It(a + 1));
    }
    {
        int a[] = {1, 1, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(Fun{}(a, a + sa) == It(a + 2));
    }
    {
        int a[] = {1, 1, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(Fun{}(a, a + sa) == It(a + sa));
    }

    {
        int a[] = {0, 0, 0, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(Fun{}(a, a + sa) == It(a + sa));
    }
    {
        int a[] = {0, 0, 0, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(Fun{}(a, a + sa) == It(a + sa));
    }
    {
        int a[] = {0, 0, 1, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(Fun{}(a, a + sa) == It(a + 3));
    }
    {
        int a[] = {0, 0, 1, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(Fun{}(a, a + sa) == It(a + sa));
    }
    {
        int a[] = {0, 1, 0, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(Fun{}(a, a + sa) == It(a + 2));
    }
    {
        int a[] = {0, 1, 0, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(Fun{}(a, a + sa) == It(a + 2));
    }
    {
        int a[] = {0, 1, 1, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(Fun{}(a, a + sa) == It(a + 3));
    }
    {
        int a[] = {0, 1, 1, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(Fun{}(a, a + sa) == It(a + sa));
    }
    {
        int a[] = {1, 0, 0, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(Fun{}(a, a + sa) == It(a + 1));
    }
    {
        int a[] = {1, 0, 0, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(Fun{}(a, a + sa) == It(a + 1));
    }
    {
        int a[] = {1, 0, 1, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(Fun{}(a, a + sa) == It(a + 1));
    }
    {
        int a[] = {1, 0, 1, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(Fun{}(a, a + sa) == It(a + 1));
    }
    {
        int a[] = {1, 1, 0, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(Fun{}(a, a + sa) == It(a + 2));
    }
    {
        int a[] = {1, 1, 0, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(Fun{}(a, a + sa) == It(a + 2));
    }
    {
        int a[] = {1, 1, 1, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(Fun{}(a, a + sa) == It(a + 3));
    }
    {
        int a[] = {1, 1, 1, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(Fun{}(a, a + sa) == It(a + sa));
    }

    {
        int a[] = {0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(Fun{}(a, a, std::greater<int>()) == It(a));
        CHECK(Fun{}(a, a + sa, std::greater<int>()) == It(a + sa));
    }
    {
        int a[] = {0, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(Fun{}(a, a + sa, std::greater<int>()) == It(a + sa));
    }
    {
        int a[] = {0, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(Fun{}(a, a + sa, std::greater<int>()) == It(a + 1));
    }
    {
        int a[] = {1, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(Fun{}(a, a + sa, std::greater<int>()) == It(a + sa));
    }
    {
        int a[] = {1, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(Fun{}(a, a + sa, std::greater<int>()) == It(a + sa));
    }

    {
        int a[] = {0, 0, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(Fun{}(a, a + sa, std::greater<int>()) == It(a + sa));
    }
    {
        int a[] = {0, 0, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(Fun{}(a, a + sa, std::greater<int>()) == It(a + 2));
    }
    {
        int a[] = {0, 1, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(Fun{}(a, a + sa, std::greater<int>()) == It(a + 1));
    }
    {
        int a[] = {0, 1, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(Fun{}(a, a + sa, std::greater<int>()) == It(a + 1));
    }
    {
        int a[] = {1, 0, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(Fun{}(a, a + sa, std::greater<int>()) == It(a + sa));
    }
    {
        int a[] = {1, 0, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(Fun{}(a, a + sa, std::greater<int>()) == It(a + 2));
    }
    {
        int a[] = {1, 1, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(Fun{}(a, a + sa, std::greater<int>()) == It(a + sa));
    }
    {
        int a[] = {1, 1, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(Fun{}(a, a + sa, std::greater<int>()) == It(a + sa));
    }

    {
        int a[] = {0, 0, 0, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(Fun{}(a, a + sa, std::greater<int>()) == It(a + sa));
    }
    {
        int a[] = {0, 0, 0, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(Fun{}(a, a + sa, std::greater<int>()) == It(a + 3));
    }
    {
        int a[] = {0, 0, 1, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(Fun{}(a, a + sa, std::greater<int>()) == It(a + 2));
    }
    {
        int a[] = {0, 0, 1, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(Fun{}(a, a + sa, std::greater<int>()) == It(a + 2));
    }
    {
        int a[] = {0, 1, 0, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(Fun{}(a, a + sa, std::greater<int>()) == It(a + 1));
    }
    {
        int a[] = {0, 1, 0, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(Fun{}(a, a + sa, std::greater<int>()) == It(a + 1));
    }
    {
        int a[] = {0, 1, 1, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(Fun{}(a, a + sa, std::greater<int>()) == It(a + 1));
    }
    {
        int a[] = {0, 1, 1, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(Fun{}(a, a + sa, std::greater<int>()) == It(a + 1));
    }
    {
        int a[] = {1, 0, 0, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(Fun{}(a, a + sa, std::greater<int>()) == It(a + sa));
    }
    {
        int a[] = {1, 0, 0, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(Fun{}(a, a + sa, std::greater<int>()) == It(a + 3));
    }
    {
        int a[] = {1, 0, 1, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(Fun{}(a, a + sa, std::greater<int>()) == It(a + 2));
    }
    {
        int a[] = {1, 0, 1, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(Fun{}(a, a + sa, std::greater<int>()) == It(a + 2));
    }
    {
        int a[] = {1, 1, 0, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(Fun{}(a, a + sa, std::greater<int>()) == It(a + sa));
    }
    {
        int a[] = {1, 1, 0, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(Fun{}(a, a + sa, std::greater<int>()) == It(a + 3));
    }
    {
        int a[] = {1, 1, 1, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(Fun{}(a, a + sa, std::greater<int>()) == It(a + sa));
    }
    {
        int a[] = {1, 1, 1, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(Fun{}(a, a + sa, std::greater<int>()) == It(a + sa));
    }
}

struct A
{
    int a;
};

int main()
{
    test<ForwardIterator<const int *>, iter_call>();
    test<BidirectionalIterator<const int *>, iter_call>();
    test<RandomAccessIterator<const int *>, iter_call>();
    test<const int *, iter_call>();

    test<ForwardIterator<const int *>, range_call>();
    test<BidirectionalIterator<const int *>, range_call>();
    test<RandomAccessIterator<const int *>, range_call>();
    test<const int *, range_call>();

    /// Initializer list test:
    {
        std::initializer_list<int> r = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
        CHECK(ranges::is_sorted_until(r) == ranges::end(r));
    }

    /// Projection test:
    {
        A as[] = {{0}, {1}, {2}, {3}, {4}};
        CHECK(ranges::is_sorted_until(as, std::less<int>{}, &A::a) == ranges::end(as));
        CHECK(ranges::is_sorted_until(as, std::greater<int>{}, &A::a) ==
              ranges::next(ranges::begin(as), 1));
    }

    /// Rvalue range test:
    {
        A as[] = {{0}, {1}, {2}, {3}, {4}};
#ifndef RANGES_WORKAROUND_MSVC_573728
        CHECK(::is_dangling(
            ranges::is_sorted_until(std::move(as), std::less<int>{}, &A::a)));
        CHECK(::is_dangling(
            ranges::is_sorted_until(std::move(as), std::greater<int>{}, &A::a)));
#endif // RANGES_WORKAROUND_MSVC_573728
        std::vector<A> vec(ranges::begin(as), ranges::end(as));
        CHECK(::is_dangling(
            ranges::is_sorted_until(std::move(vec), std::less<int>{}, &A::a)));
        CHECK(::is_dangling(
            ranges::is_sorted_until(std::move(vec), std::greater<int>{}, &A::a)));
    }

    return ::test_result();
}
