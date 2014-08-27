//  Copyright Eric Niebler 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
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

#include <range/v3/core.hpp>
#include <range/v3/algorithm/unique.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"
#include "../test_iterators.hpp"

/// Calls the iterator interface of the algorithm
template <class T, template <class> class Iter> struct iter_call
{
    using begin_t = Iter<T>;
    using sentinel_t = sentinel<T>;

    template <class B, class E, class... Args>
    auto operator()(B &&b, E &&e, Args &&... args) const
     -> decltype(ranges::unique(begin_t{b}, sentinel_t{e}, std::forward<Args>(args)...))
    {
        return ranges::unique(begin_t{b}, sentinel_t{e}, std::forward<Args>(args)...);
    }
};

/// Calls the range interface of the algorithm
template <class T, template <class> class Iter> struct range_call
{
    using begin_t = Iter<T>;
    using sentinel_t = sentinel<T>;

    template <class B, class E, class... Args>
    auto operator()(B &&b, E &&e, Args &&... args) const
     -> ranges::range_iterator_t<decltype(ranges::range(begin_t{b}, sentinel_t{e}))>
    {
        auto rng = ranges::range(begin_t{b}, sentinel_t{e});
        return ranges::unique(rng, std::forward<Args>(args)...);
    }
};

template <class T> using identity_t = T;

template <class T, template <class> class It,
          template <class, template <class> class> class F>
void test()
{
    using f = F<T, It>;
    auto b = [](T i)
    {
        return It<T>{i};
    };

    {
        int a[] = {0};
        const unsigned sa = sizeof(a) / sizeof(a[0]);
        auto r = f{}(a, a + sa);
        CHECK(r == b(a + sa));
        CHECK(a[0] == 0);
    }
    {
        int a[] = {0, 1};
        const unsigned sa = sizeof(a) / sizeof(a[0]);
        auto r = f{}(a, a + sa);
        CHECK(r == b(a + sa));
        CHECK(a[0] == 0);
        CHECK(a[1] == 1);
    }
    {
        int a[] = {0, 0};
        const unsigned sa = sizeof(a) / sizeof(a[0]);
        auto r = f{}(a, a + sa);
        CHECK(r == b(a + 1));
        CHECK(a[0] == 0);
    }
    {
        int a[] = {0, 0, 1};
        const unsigned sa = sizeof(a) / sizeof(a[0]);
        auto r = f{}(a, a + sa);
        CHECK(r == b(a + 2));
        CHECK(a[0] == 0);
        CHECK(a[1] == 1);
    }
    {
        int a[] = {0, 0, 1, 0};
        const unsigned sa = sizeof(a) / sizeof(a[0]);
        auto r = f{}(a, a + sa);
        CHECK(r == b(a + 3));
        CHECK(a[0] == 0);
        CHECK(a[1] == 1);
        CHECK(a[2] == 0);
    }
    {
        int a[] = {0, 0, 1, 1};
        const unsigned sa = sizeof(a) / sizeof(a[0]);
        auto r = f{}(a, a + sa);
        CHECK(r == b(a + 2));
        CHECK(a[0] == 0);
        CHECK(a[1] == 1);
    }
    {
        int a[] = {0, 1, 1};
        const unsigned sa = sizeof(a) / sizeof(a[0]);
        auto r = f{}(a, a + sa);
        CHECK(r == b(a + 2));
        CHECK(a[0] == 0);
        CHECK(a[1] == 1);
    }
    {
        int a[] = {0, 1, 1, 1, 2, 2, 2};
        const unsigned sa = sizeof(a) / sizeof(a[0]);
        auto r = f{}(a, a + sa);
        CHECK(r == b(a + 3));
        CHECK(a[0] == 0);
        CHECK(a[1] == 1);
        CHECK(a[2] == 2);
    }
}

int main()
{
    test<int *, forward_iterator, iter_call>();
    test<int *, bidirectional_iterator, iter_call>();
    test<int *, random_access_iterator, iter_call>();
    test<int *, identity_t, iter_call>();

    test<int *, forward_iterator, range_call>();
    test<int *, bidirectional_iterator, range_call>();
    test<int *, random_access_iterator, range_call>();
    test<int *, identity_t, range_call>();

    return ::test_result();
}
