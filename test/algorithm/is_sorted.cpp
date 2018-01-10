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

#include <range/v3/core.hpp>
#include <range/v3/algorithm/is_sorted.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"
#include "../test_iterators.hpp"

/// Calls the iterator interface of the algorithm
template<class Iter>
struct iter_call
{
    using begin_t = Iter;
    using sentinel_t = typename sentinel_type<Iter>::type;

    template<class B, class E, class... Args>
    auto operator()(B &&b, E &&e, Args &&... args)
     -> decltype(ranges::is_sorted(begin_t{b}, sentinel_t{e},
                                   std::forward<Args>(args)...))
    {
        return ranges::is_sorted(begin_t{b}, sentinel_t{e}, std::forward<Args>(args)...);
    }
};

/// Calls the range interface of the algorithm
template<class Iter>
struct range_call
{
    using begin_t = Iter;
    using sentinel_t = typename sentinel_type<Iter>::type;

    template<class B, class E, class... Args>
    auto operator()(B &&b, E &&e, Args &&... args)
     -> decltype(ranges::is_sorted(ranges::make_iterator_range(begin_t{b}, sentinel_t{e}),
                                   std::forward<Args>(args)...))
    {
        return ranges::is_sorted(ranges::make_iterator_range(begin_t{b}, sentinel_t{e}),
                                 std::forward<Args>(args)...);
    }
};

template<class Fun>
void test()
{
    {
        int a[] = {0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(Fun{}(a, a));
        CHECK(Fun{}(a, a + sa));
    }

    {
        int a[] = {0, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(Fun{}(a, a + sa));
    }
    {
        int a[] = {0, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(Fun{}(a, a + sa));
    }
    {
        int a[] = {1, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(!Fun{}(a, a + sa));
    }
    {
        int a[] = {1, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(Fun{}(a, a + sa));
    }

    {
        int a[] = {0, 0, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(Fun{}(a, a + sa));
    }
    {
        int a[] = {0, 0, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(Fun{}(a, a + sa));
    }
    {
        int a[] = {0, 1, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(!Fun{}(a, a + sa));
    }
    {
        int a[] = {0, 1, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(Fun{}(a, a + sa));
    }
    {
        int a[] = {1, 0, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(!Fun{}(a, a + sa));
    }
    {
        int a[] = {1, 0, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(!Fun{}(a, a + sa));
    }
    {
        int a[] = {1, 1, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(!Fun{}(a, a + sa));
    }
    {
        int a[] = {1, 1, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(Fun{}(a, a + sa));
    }

    {
        int a[] = {0, 0, 0, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(Fun{}(a, a + sa));
    }
    {
        int a[] = {0, 0, 0, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(Fun{}(a, a + sa));
    }
    {
        int a[] = {0, 0, 1, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(!Fun{}(a, a + sa));
    }
    {
        int a[] = {0, 0, 1, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(Fun{}(a, a + sa));
    }
    {
        int a[] = {0, 1, 0, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(!Fun{}(a, a + sa));
    }
    {
        int a[] = {0, 1, 0, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(!Fun{}(a, a + sa));
    }
    {
        int a[] = {0, 1, 1, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(!Fun{}(a, a + sa));
    }
    {
        int a[] = {0, 1, 1, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(Fun{}(a, a + sa));
    }
    {
        int a[] = {1, 0, 0, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(!Fun{}(a, a + sa));
    }
    {
        int a[] = {1, 0, 0, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(!Fun{}(a, a + sa));
    }
    {
        int a[] = {1, 0, 1, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(!Fun{}(a, a + sa));
    }
    {
        int a[] = {1, 0, 1, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(!Fun{}(a, a + sa));
    }
    {
        int a[] = {1, 1, 0, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(!Fun{}(a, a + sa));
    }
    {
        int a[] = {1, 1, 0, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(!Fun{}(a, a + sa));
    }
    {
        int a[] = {1, 1, 1, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(!Fun{}(a, a + sa));
    }
    {
        int a[] = {1, 1, 1, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(Fun{}(a, a + sa));
    }

    {
        int a[] = {0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(Fun{}(a, a));
        CHECK(Fun{}(a, a + sa, std::greater<int>()));
    }

    {
        int a[] = {0, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(Fun{}(a, a + sa, std::greater<int>()));
    }
    {
        int a[] = {0, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(!Fun{}(a, a + sa, std::greater<int>()));
    }
    {
        int a[] = {1, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(Fun{}(a, a + sa, std::greater<int>()));
    }
    {
        int a[] = {1, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(Fun{}(a, a + sa, std::greater<int>()));
    }

    {
        int a[] = {0, 0, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(Fun{}(a, a + sa, std::greater<int>()));
    }
    {
        int a[] = {0, 0, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(!Fun{}(a, a + sa, std::greater<int>()));
    }
    {
        int a[] = {0, 1, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(!Fun{}(a, a + sa, std::greater<int>()));
    }
    {
        int a[] = {0, 1, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(!Fun{}(a, a + sa, std::greater<int>()));
    }
    {
        int a[] = {1, 0, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(Fun{}(a, a + sa, std::greater<int>()));
    }
    {
        int a[] = {1, 0, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(!Fun{}(a, a + sa, std::greater<int>()));
    }
    {
        int a[] = {1, 1, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(Fun{}(a, a + sa, std::greater<int>()));
    }
    {
        int a[] = {1, 1, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(Fun{}(a, a + sa, std::greater<int>()));
    }

    {
        int a[] = {0, 0, 0, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(Fun{}(a, a + sa, std::greater<int>()));
    }
    {
        int a[] = {0, 0, 0, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(!Fun{}(a, a + sa, std::greater<int>()));
    }
    {
        int a[] = {0, 0, 1, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(!Fun{}(a, a + sa, std::greater<int>()));
    }
    {
        int a[] = {0, 0, 1, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(!Fun{}(a, a + sa, std::greater<int>()));
    }
    {
        int a[] = {0, 1, 0, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(!Fun{}(a, a + sa, std::greater<int>()));
    }
    {
        int a[] = {0, 1, 0, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(!Fun{}(a, a + sa, std::greater<int>()));
    }
    {
        int a[] = {0, 1, 1, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(!Fun{}(a, a + sa, std::greater<int>()));
    }
    {
        int a[] = {0, 1, 1, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(!Fun{}(a, a + sa, std::greater<int>()));
    }
    {
        int a[] = {1, 0, 0, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(Fun{}(a, a + sa, std::greater<int>()));
    }
    {
        int a[] = {1, 0, 0, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(!Fun{}(a, a + sa, std::greater<int>()));
    }
    {
        int a[] = {1, 0, 1, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(!Fun{}(a, a + sa, std::greater<int>()));
    }
    {
        int a[] = {1, 0, 1, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(!Fun{}(a, a + sa, std::greater<int>()));
    }
    {
        int a[] = {1, 1, 0, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(Fun{}(a, a + sa, std::greater<int>()));
    }
    {
        int a[] = {1, 1, 0, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(!Fun{}(a, a + sa, std::greater<int>()));
    }
    {
        int a[] = {1, 1, 1, 0};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(Fun{}(a, a + sa, std::greater<int>()));
    }
    {
        int a[] = {1, 1, 1, 1};
        unsigned sa = sizeof(a) / sizeof(a[0]);
        CHECK(Fun{}(a, a + sa, std::greater<int>()));
    }
}

struct A { int a; };

int main()
{
    test<iter_call<forward_iterator<const int *>>>();
    test<iter_call<bidirectional_iterator<const int *>>>();
    test<iter_call<random_access_iterator<const int *>>>();
    test<iter_call<const int *>>();

    test<range_call<forward_iterator<const int *>>>();
    test<range_call<bidirectional_iterator<const int *>>>();
    test<range_call<random_access_iterator<const int *>>>();
    test<range_call<const int *>>();

    /// Initializer list test:
    {
        CHECK(ranges::is_sorted({0,1,2,3,4,5,6,7,8,9,10}));
        CHECK(!ranges::is_sorted({0,1,2,3,5,4,6,7,8,9,10}));
    }

    /// Projection test:
    {
        A as[] = {{0}, {1}, {2}, {3}, {4}};
        CHECK(ranges::is_sorted(as, std::less<int>{}, &A::a));
        CHECK(!ranges::is_sorted(as, std::greater<int>{}, &A::a));
    }

    return ::test_result();
}
