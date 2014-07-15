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

#include <random>
#include <range/v3/core.hpp>
#include <range/v3/algorithm/unique.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"
#include "../test_iterators.hpp"

/// These are libc++ tests:
template <class Iter> void test_iterators()
{
    auto b = [](int *i)
    {
        return Iter(i);
    };
    auto s = [](int *i)
    {
        return sentinel<int *>(i);
    };
    {
        static const constexpr unsigned N = 10;
        int a_ref[N] = {1, 1, 2, 2, 3, 4, 4, 5, 5, 6};
        static const constexpr unsigned unique_end = 6;

        std::unique(std::begin(a_ref), std::end(a_ref));

        int a[N] = {1, 1, 2, 2, 3, 4, 4, 5, 5, 6};

        auto ret = ranges::unique(b(a), s(a + N));
        check_equal(a, a_ref);
        CHECK(&*ret == &a[unique_end]);
        CHECK(*ret == a[unique_end]);
    }
    {
        int ia[] = {0};
        const unsigned sa = sizeof(ia) / sizeof(ia[0]);
        auto r = ranges::unique(b(ia), s(ia + sa));
        CHECK(r == s(ia + sa));
        CHECK(ia[0] == 0);
    }
    {
        int ib[] = {0, 1};
        const unsigned sb = sizeof(ib) / sizeof(ib[0]);
        auto r = ranges::unique(b(ib), s(ib + sb));
        CHECK(base(r) == ib + sb);
        CHECK(ib[0] == 0);
        CHECK(ib[1] == 1);
    }
    {
        int ic[] = {0, 0};
        const unsigned sc = sizeof(ic) / sizeof(ic[0]);
        auto r = ranges::unique(b(ic), s(ic + sc));
        CHECK(base(r) == ic + 1);
        CHECK(ic[0] == 0);
    }
    {
        int id[] = {0, 0, 1};
        const unsigned sd = sizeof(id) / sizeof(id[0]);
        auto r = ranges::unique(b(id), s(id + sd));
        CHECK(base(r) == id + 2);
        CHECK(id[0] == 0);
        CHECK(id[1] == 1);
    }
    {
        int ie[] = {0, 0, 1, 0};
        const unsigned se = sizeof(ie) / sizeof(ie[0]);
        auto r = ranges::unique(b(ie), s(ie + se));
        CHECK(base(r) == ie + 3);
        CHECK(ie[0] == 0);
        CHECK(ie[1] == 1);
        CHECK(ie[2] == 0);
    }
    {
        int ig[] = {0, 0, 1, 1};
        const unsigned sg = sizeof(ig) / sizeof(ig[0]);
        auto r = ranges::unique(b(ig), s(ig + sg));
        CHECK(base(r) == ig + 2);
        CHECK(ig[0] == 0);
        CHECK(ig[1] == 1);
    }
    {
        int ih[] = {0, 1, 1};
        const unsigned sh = sizeof(ih) / sizeof(ih[0]);
        auto r = ranges::unique(b(ih), s(ih + sh));
        CHECK(base(r) == ih + 2);
        CHECK(ih[0] == 0);
        CHECK(ih[1] == 1);
    }
    {
        int ii[] = {0, 1, 1, 1, 2, 2, 2};
        const unsigned si = sizeof(ii) / sizeof(ii[0]);
        auto r = ranges::unique(b(ii), s(ii + si));
        CHECK(base(r) == ii + 3);
        CHECK(ii[0] == 0);
        CHECK(ii[1] == 1);
        CHECK(ii[2] == 2);
    }
}

template <class Iter> void test_range()
{
    auto b = [](int *i)
    {
        return Iter(i);
    };
    auto s = [](int *i)
    {
        return sentinel<int *>(i);
    };

    {
        static const constexpr unsigned N = 10;
        int a_ref[N] = {1, 1, 2, 2, 3, 4, 4, 5, 5, 6};
        static const constexpr unsigned unique_end = 6;

        std::unique(std::begin(a_ref), std::end(a_ref));

        int a[N] = {1, 1, 2, 2, 3, 4, 4, 5, 5, 6};
        auto rng = ranges::range(b(a), s(a + N));

        auto ret = ranges::unique(rng);
        check_equal(a, a_ref);
        CHECK(&*ret == &a[unique_end]);
        CHECK(*ret == a[unique_end]);
    }
    {
        int ia[] = {0};
        const unsigned sa = sizeof(ia) / sizeof(ia[0]);
        auto rnga = ranges::range(b(ia), s(ia + sa));
        auto r = ranges::unique(rnga);
        CHECK(r == s(ia + sa));
        CHECK(ia[0] == 0);
    }
    {
        int ib[] = {0, 1};
        const unsigned sb = sizeof(ib) / sizeof(ib[0]);
        auto rngb = ranges::range(b(ib), s(ib + sb));
        auto r = ranges::unique(rngb);
        CHECK(base(r) == ib + sb);
        CHECK(ib[0] == 0);
        CHECK(ib[1] == 1);
    }
    {
        int ic[] = {0, 0};
        const unsigned sc = sizeof(ic) / sizeof(ic[0]);
        auto rngc = ranges::range(b(ic), s(ic + sc));
        auto r = ranges::unique(rngc);
        CHECK(base(r) == ic + 1);
        CHECK(ic[0] == 0);
    }
    {
        int id[] = {0, 0, 1};
        const unsigned sd = sizeof(id) / sizeof(id[0]);
        auto rngd = ranges::range(b(id), s(id + sd));
        auto r = ranges::unique(rngd);
        CHECK(base(r) == id + 2);
        CHECK(id[0] == 0);
        CHECK(id[1] == 1);
    }
    {
        int ie[] = {0, 0, 1, 0};
        const unsigned se = sizeof(ie) / sizeof(ie[0]);
        auto rnge = ranges::range(b(ie), s(ie + se));
        auto r = ranges::unique(rnge);
        CHECK(base(r) == ie + 3);
        CHECK(ie[0] == 0);
        CHECK(ie[1] == 1);
        CHECK(ie[2] == 0);
    }
    {
        int ig[] = {0, 0, 1, 1};
        const unsigned sg = sizeof(ig) / sizeof(ig[0]);
        auto rngg = ranges::range(b(ig), s(ig + sg));
        auto r = ranges::unique(rngg);
        CHECK(base(r) == ig + 2);
        CHECK(ig[0] == 0);
        CHECK(ig[1] == 1);
    }
    {
        int ih[] = {0, 1, 1};
        const unsigned sh = sizeof(ih) / sizeof(ih[0]);
        auto rngh = ranges::range(b(ih), s(ih + sh));
        auto r = ranges::unique(rngh);
        CHECK(base(r) == ih + 2);
        CHECK(ih[0] == 0);
        CHECK(ih[1] == 1);
    }
    {
        int ii[] = {0, 1, 1, 1, 2, 2, 2};
        const unsigned si = sizeof(ii) / sizeof(ii[0]);
        auto rngi = ranges::range(b(ii), s(ii + si));
        auto r = ranges::unique(rngi);
        CHECK(base(r) == ii + 3);
        CHECK(ii[0] == 0);
        CHECK(ii[1] == 1);
        CHECK(ii[2] == 2);
    }
}

int main()
{
    test_iterators<forward_iterator<int *>>();
    test_iterators<bidirectional_iterator<int *>>();
    test_iterators<random_access_iterator<int *>>();
    test_iterators<int *>();

    test_range<forward_iterator<int *>>();
    test_range<bidirectional_iterator<int *>>();
    test_range<random_access_iterator<int *>>();
    test_range<int *>();

    return ::test_result();
}
