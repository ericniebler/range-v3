// Range v3 library
//
//  Copyright Eric Niebler 2014
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

#include <range/v3/core.hpp>
#include <range/v3/algorithm/is_sorted_until.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"
#include "../test_iterators.hpp"

/// Calls the iterator interface of the algorithm
template <class T, template <class> class Iter> struct iter_call
{
    using begin_t = Iter<T>;
    using sentinel_t = sentinel<T>;

    template <class B, class E, class... Args>
    auto operator()(B &&b, E &&e, Args &&... args)
     -> decltype(ranges::is_sorted_until(begin_t{b}, sentinel_t{e},
                                         std::forward<Args>(args)...))
    {
        return ranges::is_sorted_until(begin_t{b}, sentinel_t{e},
                                       std::forward<Args>(args)...);
    }
};

/// Calls the range interface of the algorithm
template <class T, template <class> class Iter> struct range_call
{
    using begin_t = Iter<T>;
    using sentinel_t = sentinel<T>;

    template <class B, class E, class... Args>
    auto operator()(B &&b, E &&e, Args &&... args)
     -> decltype(ranges::is_sorted_until(::as_lvalue(ranges::make_range(begin_t{b}, sentinel_t{e})),
                                         std::forward<Args>(args)...))
    {
        return ranges::is_sorted_until(::as_lvalue(ranges::make_range(begin_t{b}, sentinel_t{e})),
                                       std::forward<Args>(args)...);
    }
};

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
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(f{}(a, a) == b(a));
        CHECK(f{}(a, a + sa) == b(a + sa));
    }

    {
        int a[] = {0, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(f{}(a, a + sa) == b(a + sa));
    }
    {
        int a[] = {0, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(f{}(a, a + sa) == b(a + sa));
    }
    {
        int a[] = {1, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(f{}(a, a + sa) == b(a + 1));
    }
    {
        int a[] = {1, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(f{}(a, a + sa) == b(a + sa));
    }

    {
        int a[] = {0, 0, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(f{}(a, a + sa) == b(a + sa));
    }
    {
        int a[] = {0, 0, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(f{}(a, a + sa) == b(a + sa));
    }
    {
        int a[] = {0, 1, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(f{}(a, a + sa) == b(a + 2));
    }
    {
        int a[] = {0, 1, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(f{}(a, a + sa) == b(a + sa));
    }
    {
        int a[] = {1, 0, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(f{}(a, a + sa) == b(a + 1));
    }
    {
        int a[] = {1, 0, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(f{}(a, a + sa) == b(a + 1));
    }
    {
        int a[] = {1, 1, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(f{}(a, a + sa) == b(a + 2));
    }
    {
        int a[] = {1, 1, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(f{}(a, a + sa) == b(a + sa));
    }

    {
        int a[] = {0, 0, 0, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(f{}(a, a + sa) == b(a + sa));
    }
    {
        int a[] = {0, 0, 0, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(f{}(a, a + sa) == b(a + sa));
    }
    {
        int a[] = {0, 0, 1, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(f{}(a, a + sa) == b(a + 3));
    }
    {
        int a[] = {0, 0, 1, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(f{}(a, a + sa) == b(a + sa));
    }
    {
        int a[] = {0, 1, 0, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(f{}(a, a + sa) == b(a + 2));
    }
    {
        int a[] = {0, 1, 0, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(f{}(a, a + sa) == b(a + 2));
    }
    {
        int a[] = {0, 1, 1, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(f{}(a, a + sa) == b(a + 3));
    }
    {
        int a[] = {0, 1, 1, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(f{}(a, a + sa) == b(a + sa));
    }
    {
        int a[] = {1, 0, 0, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(f{}(a, a + sa) == b(a + 1));
    }
    {
        int a[] = {1, 0, 0, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(f{}(a, a + sa) == b(a + 1));
    }
    {
        int a[] = {1, 0, 1, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(f{}(a, a + sa) == b(a + 1));
    }
    {
        int a[] = {1, 0, 1, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(f{}(a, a + sa) == b(a + 1));
    }
    {
        int a[] = {1, 1, 0, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(f{}(a, a + sa) == b(a + 2));
    }
    {
        int a[] = {1, 1, 0, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(f{}(a, a + sa) == b(a + 2));
    }
    {
        int a[] = {1, 1, 1, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(f{}(a, a + sa) == b(a + 3));
    }
    {
        int a[] = {1, 1, 1, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(f{}(a, a + sa) == b(a + sa));
    }

    {
        int a[] = {0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(f{}(a, a, std::greater<int>()) == b(a));
        CHECK(f{}(a, a + sa, std::greater<int>()) == b(a + sa));
    }
    {
        int a[] = {0, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(f{}(a, a + sa, std::greater<int>()) == b(a + sa));
    }
    {
        int a[] = {0, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(f{}(a, a + sa, std::greater<int>()) == b(a + 1));
    }
    {
        int a[] = {1, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(f{}(a, a + sa, std::greater<int>()) == b(a + sa));
    }
    {
        int a[] = {1, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(f{}(a, a + sa, std::greater<int>()) == b(a + sa));
    }

    {
        int a[] = {0, 0, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(f{}(a, a + sa, std::greater<int>()) == b(a + sa));
    }
    {
        int a[] = {0, 0, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(f{}(a, a + sa, std::greater<int>()) == b(a + 2));
    }
    {
        int a[] = {0, 1, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(f{}(a, a + sa, std::greater<int>()) == b(a + 1));
    }
    {
        int a[] = {0, 1, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(f{}(a, a + sa, std::greater<int>()) == b(a + 1));
    }
    {
        int a[] = {1, 0, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(f{}(a, a + sa, std::greater<int>()) == b(a + sa));
    }
    {
        int a[] = {1, 0, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(f{}(a, a + sa, std::greater<int>()) == b(a + 2));
    }
    {
        int a[] = {1, 1, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(f{}(a, a + sa, std::greater<int>()) == b(a + sa));
    }
    {
        int a[] = {1, 1, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(f{}(a, a + sa, std::greater<int>()) == b(a + sa));
    }

    {
        int a[] = {0, 0, 0, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(f{}(a, a + sa, std::greater<int>()) == b(a + sa));
    }
    {
        int a[] = {0, 0, 0, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(f{}(a, a + sa, std::greater<int>()) == b(a + 3));
    }
    {
        int a[] = {0, 0, 1, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(f{}(a, a + sa, std::greater<int>()) == b(a + 2));
    }
    {
        int a[] = {0, 0, 1, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(f{}(a, a + sa, std::greater<int>()) == b(a + 2));
    }
    {
        int a[] = {0, 1, 0, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(f{}(a, a + sa, std::greater<int>()) == b(a + 1));
    }
    {
        int a[] = {0, 1, 0, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(f{}(a, a + sa, std::greater<int>()) == b(a + 1));
    }
    {
        int a[] = {0, 1, 1, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(f{}(a, a + sa, std::greater<int>()) == b(a + 1));
    }
    {
        int a[] = {0, 1, 1, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(f{}(a, a + sa, std::greater<int>()) == b(a + 1));
    }
    {
        int a[] = {1, 0, 0, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(f{}(a, a + sa, std::greater<int>()) == b(a + sa));
    }
    {
        int a[] = {1, 0, 0, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(f{}(a, a + sa, std::greater<int>()) == b(a + 3));
    }
    {
        int a[] = {1, 0, 1, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(f{}(a, a + sa, std::greater<int>()) == b(a + 2));
    }
    {
        int a[] = {1, 0, 1, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(f{}(a, a + sa, std::greater<int>()) == b(a + 2));
    }
    {
        int a[] = {1, 1, 0, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(f{}(a, a + sa, std::greater<int>()) == b(a + sa));
    }
    {
        int a[] = {1, 1, 0, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(f{}(a, a + sa, std::greater<int>()) == b(a + 3));
    }
    {
        int a[] = {1, 1, 1, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(f{}(a, a + sa, std::greater<int>()) == b(a + sa));
    }
    {
        int a[] = {1, 1, 1, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(f{}(a, a + sa, std::greater<int>()) == b(a + sa));
    }
}

template <class T> using identity_t = T;

struct A { int a; };

int main()
{
    test<const int *, forward_iterator, iter_call>();
    test<const int *, bidirectional_iterator, iter_call>();
    test<const int *, random_access_iterator, iter_call>();
    test<const int *, identity_t, iter_call>();

    test<const int *, forward_iterator, range_call>();
    test<const int *, bidirectional_iterator, range_call>();
    test<const int *, random_access_iterator, range_call>();
    test<const int *, identity_t, range_call>();

    /// Initializer list test:
    {
        std::initializer_list<int> r = {0,1,2,3,4,5,6,7,8,9,10};
        CHECK(ranges::is_sorted_until(r) == ranges::end(r));
    }

    /// Projection test:
    {
        A as[] = {{0}, {1}, {2}, {3}, {4}};
        CHECK(ranges::is_sorted_until(as, std::less<int>{}, &A::a) == ranges::end(as));
        CHECK(ranges::is_sorted_until(as, std::greater<int>{}, &A::a) == ranges::next(ranges::begin(as),1));
    }

    return ::test_result();
}
