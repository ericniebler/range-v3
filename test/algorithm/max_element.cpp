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
#include <numeric>
#include <random>
#include <algorithm>
#include <range/v3/core.hpp>
#include <range/v3/algorithm/max_element.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"
#include "../test_iterators.hpp"

RANGES_DIAGNOSTIC_IGNORE_GLOBAL_CONSTRUCTORS

namespace
{
    std::mt19937 gen;

    template<class Iter, class Sent = Iter>
    void
    test_iter(Iter first, Sent last)
    {
        Iter i = ranges::max_element(first, last);
        if (first != last)
        {
            for (Iter j = first; j != last; ++j)
                CHECK(!(*i < *j));
        }
        else
            CHECK(i == last);

        auto rng = ranges::make_iterator_range(first, last);
        i = ranges::max_element(rng);
        if (first != last)
        {
            for (Iter j = first; j != last; ++j)
                CHECK(!(*i < *j));
        }
        else
            CHECK(i == last);

        auto j = ranges::max_element(std::move(rng));
        if (first != last)
        {
            for (Iter k = first; k != last; ++k)
                CHECK(!(*j.get_unsafe() < *k));
        }
        else
            CHECK(j.get_unsafe() == last);
    }

    template<class Iter, class Sent = Iter>
    void
    test_iter(unsigned N)
    {
        std::unique_ptr<int[]> a{new int[N]};
        std::iota(a.get(), a.get()+N, 0);
        std::shuffle(a.get(), a.get()+N, gen);
        test_iter(Iter(a.get()), Sent(a.get()+N));
    }

    template<class Iter, class Sent = Iter>
    void
    test_iter()
    {
        test_iter<Iter, Sent>(0);
        test_iter<Iter, Sent>(1);
        test_iter<Iter, Sent>(2);
        test_iter<Iter, Sent>(3);
        test_iter<Iter, Sent>(10);
        test_iter<Iter, Sent>(1000);
    }

    template<class Iter, class Sent = Iter>
    void
    test_iter_comp(Iter first, Sent last)
    {
        Iter i = ranges::max_element(first, last, std::greater<int>());
        if (first != last)
        {
            for (Iter j = first; j != last; ++j)
                CHECK(!std::greater<int>()(*i, *j));
        }
        else
            CHECK(i == last);

        auto rng = ranges::make_iterator_range(first, last);
        i = ranges::max_element(rng, std::greater<int>());
        if (first != last)
        {
            for (Iter j = first; j != last; ++j)
                CHECK(!std::greater<int>()(*i, *j));
        }
        else
            CHECK(i == last);

        auto res = ranges::max_element(std::move(rng), std::greater<int>());
        if (first != last)
        {
            for (Iter j = first; j != last; ++j)
                CHECK(!std::greater<int>()(*res.get_unsafe(), *j));
        }
        else
            CHECK(res.get_unsafe() == last);
    }

    template<class Iter, class Sent = Iter>
    void
    test_iter_comp(unsigned N)
    {
        std::unique_ptr<int[]> a{new int[N]};
        std::iota(a.get(), a.get()+N, 0);
        std::shuffle(a.get(), a.get()+N, gen);
        test_iter_comp(Iter(a.get()), Sent(a.get()+N));
    }

    template<class Iter, class Sent = Iter>
    void
    test_iter_comp()
    {
        test_iter_comp<Iter, Sent>(0);
        test_iter_comp<Iter, Sent>(1);
        test_iter_comp<Iter, Sent>(2);
        test_iter_comp<Iter, Sent>(3);
        test_iter_comp<Iter, Sent>(10);
        test_iter_comp<Iter, Sent>(1000);
    }

    struct S
    {
        int i;
    };
}

int main()
{
    test_iter<forward_iterator<const int*> >();
    test_iter<bidirectional_iterator<const int*> >();
    test_iter<random_access_iterator<const int*> >();
    test_iter<const int*>();
    test_iter<forward_iterator<const int*>, sentinel<const int*>>();
    test_iter<bidirectional_iterator<const int*>, sentinel<const int*>>();
    test_iter<random_access_iterator<const int*>, sentinel<const int*>>();

    test_iter_comp<forward_iterator<const int*> >();
    test_iter_comp<bidirectional_iterator<const int*> >();
    test_iter_comp<random_access_iterator<const int*> >();
    test_iter_comp<const int*>();
    test_iter_comp<forward_iterator<const int*>, sentinel<const int*>>();
    test_iter_comp<bidirectional_iterator<const int*>, sentinel<const int*>>();
    test_iter_comp<random_access_iterator<const int*>, sentinel<const int*>>();

    // Works with projections?
    S s[] = {S{1},S{2},S{3},S{4},S{40},S{5},S{6},S{7},S{8},S{9}};
    S const *ps = ranges::max_element(s, std::less<int>{}, &S::i);
    CHECK(ps->i == 40);

    return test_result();
}
