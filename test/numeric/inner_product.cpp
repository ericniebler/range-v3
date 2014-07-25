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
    S add(int j) const
    {
        return S{i + j};
    }
};

template <class Iter1, class Iter2, class T, class Sent1 = Iter1>
bool test(Iter1 first1, Sent1 last1, Iter2 first2, T init, T x)
{
    return ranges::inner_product(first1, last1, first2, init) == x;
}

template <class Rng1, class Rng2, class T>
bool test(Rng1 rng1, Rng2 rng2, T init, T x)
{
    return ranges::inner_product(rng1, rng2, init) == x;
}

template <class Iter1, class Iter2, class Sent1 = Iter1>
void test()
{
  using ranges::inner_product;
    int a[] = {1, 2, 3, 4, 5, 6};
    int b[] = {6, 5, 4, 3, 2, 1};
    unsigned sa = sizeof(a) / sizeof(a[0]);

    // iterator test:
    CHECK(inner_product(Iter1(a), Iter1(a), Iter2(b), 0) == 0);
    CHECK(inner_product(Iter1(a), Iter1(a), Iter2(b), 10) == 10);
    CHECK(inner_product(Iter1(a), Iter1(a+1), Iter2(b), 0) == 6);
    CHECK(inner_product(Iter1(a), Iter1(a+1), Iter2(b), 10) == 16);
    CHECK(inner_product(Iter1(a), Iter1(a+2), Iter2(b), 0) == 16);
    CHECK(inner_product(Iter1(a), Iter1(a+2), Iter2(b), 10) == 26);
    CHECK(inner_product(Iter1(a), Iter1(a+sa), Iter2(b), 0) == 56);
    CHECK(inner_product(Iter1(a), Iter1(a+sa), Iter2(b), 10) ==  66);

    // rng test:
    using ranges::range;
    CHECK(inner_product(range(Iter1(a), Sent1(a)), range(Iter2(b), Iter2(b)), 0) == 0);
    CHECK(inner_product(range(Iter1(a), Sent1(a)), range(Iter2(b), Iter2(b)), 10) == 10);
    CHECK(inner_product(range(Iter1(a), Sent1(a+1)), range(Iter2(b), Iter2(b+1)), 0) == 6);
    CHECK(inner_product(range(Iter1(a), Sent1(a+1)), range(Iter2(b), Iter2(b+1)), 10) == 16);
    CHECK(inner_product(range(Iter1(a), Sent1(a+2)), range(Iter2(b), Iter2(b+2)), 0) == 16);
    CHECK(inner_product(range(Iter1(a), Sent1(a+2)), range(Iter2(b), Iter2(b+2)), 10) == 26);
    CHECK(inner_product(range(Iter1(a), Sent1(a+sa)), range(Iter2(b), Iter2(b+sa)), 0) == 56);
    CHECK(inner_product(range(Iter1(a), Sent1(a+sa)), range(Iter2(b), Iter2(b+sa)), 10) == 66);

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
}
