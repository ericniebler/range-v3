//  Copyright Eric Niebler 2014
//  Copyright Gonzalo Brito Gadeschi 2014
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
#include <range/v3/algorithm/is_sorted.hpp>
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
     -> decltype(ranges::is_sorted(begin_t{b}, sentinel_t{e},
                                   std::forward<Args>(args)...))
    {
        return ranges::is_sorted(begin_t{b}, sentinel_t{e}, std::forward<Args>(args)...);
    }
};

/// Calls the range interface of the algorithm
template <class T, template <class> class Iter> struct range_call
{
    using begin_t = Iter<T>;
    using sentinel_t = sentinel<T>;

    template <class B, class E, class... Args>
    auto operator()(B &&b, E &&e, Args &&... args)
     -> decltype(ranges::is_sorted(ranges::range(begin_t{b}, sentinel_t{e}),
                                   std::forward<Args>(args)...))
    {
        return ranges::is_sorted(ranges::range(begin_t{b}, sentinel_t{e}),
                                 std::forward<Args>(args)...);
    }
};

template <class T, template <class> class It,
          template <class, template <class> class> class F>
void test()
{
    using f = F<T, It>;

    {
        int a[] = {0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(f{}(a, a));
        CHECK(f{}(a, a + sa));
    }

    {
        int a[] = {0, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(f{}(a, a + sa));
    }
    {
        int a[] = {0, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(f{}(a, a + sa));
    }
    {
        int a[] = {1, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(!f{}(a, a + sa));
    }
    {
        int a[] = {1, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(f{}(a, a + sa));
    }

    {
        int a[] = {0, 0, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(f{}(a, a + sa));
    }
    {
        int a[] = {0, 0, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(f{}(a, a + sa));
    }
    {
        int a[] = {0, 1, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(!f{}(a, a + sa));
    }
    {
        int a[] = {0, 1, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(f{}(a, a + sa));
    }
    {
        int a[] = {1, 0, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(!f{}(a, a + sa));
    }
    {
        int a[] = {1, 0, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(!f{}(a, a + sa));
    }
    {
        int a[] = {1, 1, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(!f{}(a, a + sa));
    }
    {
        int a[] = {1, 1, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(f{}(a, a + sa));
    }

    {
        int a[] = {0, 0, 0, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(f{}(a, a + sa));
    }
    {
        int a[] = {0, 0, 0, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(f{}(a, a + sa));
    }
    {
        int a[] = {0, 0, 1, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(!f{}(a, a + sa));
    }
    {
        int a[] = {0, 0, 1, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(f{}(a, a + sa));
    }
    {
        int a[] = {0, 1, 0, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(!f{}(a, a + sa));
    }
    {
        int a[] = {0, 1, 0, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(!f{}(a, a + sa));
    }
    {
        int a[] = {0, 1, 1, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(!f{}(a, a + sa));
    }
    {
        int a[] = {0, 1, 1, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(f{}(a, a + sa));
    }
    {
        int a[] = {1, 0, 0, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(!f{}(a, a + sa));
    }
    {
        int a[] = {1, 0, 0, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(!f{}(a, a + sa));
    }
    {
        int a[] = {1, 0, 1, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(!f{}(a, a + sa));
    }
    {
        int a[] = {1, 0, 1, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(!f{}(a, a + sa));
    }
    {
        int a[] = {1, 1, 0, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(!f{}(a, a + sa));
    }
    {
        int a[] = {1, 1, 0, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(!f{}(a, a + sa));
    }
    {
        int a[] = {1, 1, 1, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(!f{}(a, a + sa));
    }
    {
        int a[] = {1, 1, 1, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(f{}(a, a + sa));
    }

    {
        int a[] = {0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(f{}(a, a));
        CHECK(f{}(a, a + sa, std::greater<int>()));
    }

    {
        int a[] = {0, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(f{}(a, a + sa, std::greater<int>()));
    }
    {
        int a[] = {0, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(!f{}(a, a + sa, std::greater<int>()));
    }
    {
        int a[] = {1, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(f{}(a, a + sa, std::greater<int>()));
    }
    {
        int a[] = {1, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(f{}(a, a + sa, std::greater<int>()));
    }

    {
        int a[] = {0, 0, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(f{}(a, a + sa, std::greater<int>()));
    }
    {
        int a[] = {0, 0, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(!f{}(a, a + sa, std::greater<int>()));
    }
    {
        int a[] = {0, 1, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(!f{}(a, a + sa, std::greater<int>()));
    }
    {
        int a[] = {0, 1, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(!f{}(a, a + sa, std::greater<int>()));
    }
    {
        int a[] = {1, 0, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(f{}(a, a + sa, std::greater<int>()));
    }
    {
        int a[] = {1, 0, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(!f{}(a, a + sa, std::greater<int>()));
    }
    {
        int a[] = {1, 1, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(f{}(a, a + sa, std::greater<int>()));
    }
    {
        int a[] = {1, 1, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(f{}(a, a + sa, std::greater<int>()));
    }

    {
        int a[] = {0, 0, 0, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(f{}(a, a + sa, std::greater<int>()));
    }
    {
        int a[] = {0, 0, 0, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(!f{}(a, a + sa, std::greater<int>()));
    }
    {
        int a[] = {0, 0, 1, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(!f{}(a, a + sa, std::greater<int>()));
    }
    {
        int a[] = {0, 0, 1, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(!f{}(a, a + sa, std::greater<int>()));
    }
    {
        int a[] = {0, 1, 0, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(!f{}(a, a + sa, std::greater<int>()));
    }
    {
        int a[] = {0, 1, 0, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(!f{}(a, a + sa, std::greater<int>()));
    }
    {
        int a[] = {0, 1, 1, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(!f{}(a, a + sa, std::greater<int>()));
    }
    {
        int a[] = {0, 1, 1, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(!f{}(a, a + sa, std::greater<int>()));
    }
    {
        int a[] = {1, 0, 0, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(f{}(a, a + sa, std::greater<int>()));
    }
    {
        int a[] = {1, 0, 0, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(!f{}(a, a + sa, std::greater<int>()));
    }
    {
        int a[] = {1, 0, 1, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(!f{}(a, a + sa, std::greater<int>()));
    }
    {
        int a[] = {1, 0, 1, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(!f{}(a, a + sa, std::greater<int>()));
    }
    {
        int a[] = {1, 1, 0, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(f{}(a, a + sa, std::greater<int>()));
    }
    {
        int a[] = {1, 1, 0, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(!f{}(a, a + sa, std::greater<int>()));
    }
    {
        int a[] = {1, 1, 1, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(f{}(a, a + sa, std::greater<int>()));
    }
    {
        int a[] = {1, 1, 1, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(f{}(a, a + sa, std::greater<int>()));
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
      CHECK(ranges::is_sorted({0,1,2,3,4,5,6,7,8,9,10}));
      CHECK(!ranges::is_sorted({0,1,2,3,5,4,6,7,8,9,10}));
    }

    /// Projection test:
    {
      A as[] = {{0}, {1}, {2}, {3}, {4}};
      CHECK(ranges::is_sorted(as, std::less<>{}, &A::a));
      CHECK(!ranges::is_sorted(as, std::greater<>{}, &A::a)); 
    }

    return ::test_result();
}
