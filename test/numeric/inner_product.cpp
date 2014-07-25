//  Copyright Eric Niebler 2014.
//  Copyright Gonzalo Brito Gadeschi 2014.
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//
// Implementation based on the code in libc++
//   http://http://libcxx.llvm.org/

//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include <range/v3/core.hpp>
#include <range/v3/numeric/inner_product.hpp>
#include <range/v3/algorithm/equal.hpp>
#include "../simple_test.hpp"
#include "../test_iterators.hpp"

struct S
{
    int i;
};

template <class Iter1, class Iter2, class Sent1 = Iter1>
void test()
{
    int a[] = {1, 2, 3, 4, 5, 6};
    int b[] = {6, 5, 4, 3, 2, 1};
    unsigned sa = sizeof(a) / sizeof(a[0]);

    // iterator test:
    auto it = [&](auto b1, auto l1, auto b2, auto i) {
      return ranges::inner_product(Iter1(b1), Sent1(b1+l1), Iter2(b2), i);
    };
    CHECK(it(a, 0, b, 0) == 0);
    CHECK(it(a, 0, b, 10) == 10);
    CHECK(it(a, 1, b, 0) == 6);
    CHECK(it(a, 1, b, 10) == 16);
    CHECK(it(a, 2, b, 0) == 16);
    CHECK(it(a, 2, b, 10) == 26);
    CHECK(it(a, sa, b, 0) == 56);
    CHECK(it(a, sa, b, 10) == 66);

    // rng test:
    auto rng = [&](auto b1, auto l1, auto b2, auto i) {
      return ranges::inner_product(ranges::range(Iter1(b1), Sent1(b1+l1)),
                                   ranges::range(Iter2(b2), Iter2(b2+l1)), i);
    };
    CHECK(rng(a, 0, b, 0) == 0);
    CHECK(rng(a, 0, b, 10)  == 10);
    CHECK(rng(a, 1, b, 0) == 6);
    CHECK(rng(a, 1, b, 10) == 16);
    CHECK(rng(a, 2, b, 0) == 16);
    CHECK(rng(a, 2, b, 10) == 26);
    CHECK(rng(a, sa, b, 0) == 56);
    CHECK(rng(a, sa, b, 10) == 66);

    // rng + bops:
    auto bops = [&](auto b1, auto l1, auto b2, auto i) {
      return ranges::inner_product(ranges::range(Iter1(b1), Sent1(b1+l1)),
                                   ranges::range(Iter2(b2), Iter2(b2+l1)), i,
                                   std::multiplies<>(), std::plus<>());
    };
    CHECK(bops(a, 0, b, 1) == 1);
    CHECK(bops(a, 0, b, 10) == 10);
    CHECK(bops(a, 1, b, 1) == 7);
    CHECK(bops(a, 1, b, 10) == 70);
    CHECK(bops(a, 2, b, 1) == 49);
    CHECK(bops(a, 2, b, 10) == 490);
    CHECK(bops(a, sa, b, 1) == 117649);
    CHECK(bops(a, sa, b, 10) == 1176490);
}


int main()
{
    test<input_iterator<const int*>, input_iterator<const int*> >();
    test<input_iterator<const int*>, forward_iterator<const int*> >();
    test<input_iterator<const int*>, bidirectional_iterator<const int*> >();
    test<input_iterator<const int*>, random_access_iterator<const int*> >();
    test<input_iterator<const int*>, const int*>();

    test<forward_iterator<const int*>, input_iterator<const int*> >();
    test<forward_iterator<const int*>, forward_iterator<const int*> >();
    test<forward_iterator<const int*>, bidirectional_iterator<const int*> >();
    test<forward_iterator<const int*>, random_access_iterator<const int*> >();
    test<forward_iterator<const int*>, const int*>();

    test<bidirectional_iterator<const int*>, input_iterator<const int*> >();
    test<bidirectional_iterator<const int*>, forward_iterator<const int*> >();
    test<bidirectional_iterator<const int*>, bidirectional_iterator<const int*> >();
    test<bidirectional_iterator<const int*>, random_access_iterator<const int*> >();
    test<bidirectional_iterator<const int*>, const int*>();

    test<random_access_iterator<const int*>, input_iterator<const int*> >();
    test<random_access_iterator<const int*>, forward_iterator<const int*> >();
    test<random_access_iterator<const int*>, bidirectional_iterator<const int*> >();
    test<random_access_iterator<const int*>, random_access_iterator<const int*> >();
    test<random_access_iterator<const int*>, const int*>();

    test<const int*, input_iterator<const int*> >();
    test<const int*, forward_iterator<const int*> >();
    test<const int*, bidirectional_iterator<const int*> >();
    test<const int*, random_access_iterator<const int*> >();
    test<const int*, const int*>();


    // Test initializer lists:
    CHECK(ranges::inner_product({1,2,3}, {4,5,6}, 0) == 32);
    return ::test_result();

    // test projections:
    {
      S a[] = {{1}, {2}, {3}, {4}, {5}, {6}};
      S b[] = {{6}, {5}, {4}, {3}, {2}, {1}};
      unsigned sa = sizeof(a) / sizeof(a[0]);

      using Iter1 = input_iterator<const S*>;
      using Sent1 = input_iterator<const S*>;
      using Iter2 = Iter1;

      // rng + bops:
      auto bops = [&](auto b1, auto l1, auto b2, auto i) {
        return ranges::inner_product(ranges::range(Iter1(b1), Sent1(b1+l1)),
                                     ranges::range(Iter2(b2), Iter2(b2+l1)), i,
                                     std::multiplies<>(), std::plus<>(),
                                     &S::i, &S::i);
      };

      CHECK(bops(a, 0, b, 1) == 1);
      CHECK(bops(a, 0, b, 10) == 10);
      CHECK(bops(a, 1, b, 1) == 7);
      CHECK(bops(a, 1, b, 10) == 70);
      CHECK(bops(a, 2, b, 1) == 49);
      CHECK(bops(a, 2, b, 10) == 490);
      CHECK(bops(a, sa, b, 1) == 117649);
      CHECK(bops(a, sa, b, 10) == 1176490);
    }
}
