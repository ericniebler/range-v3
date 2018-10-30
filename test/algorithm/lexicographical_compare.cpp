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

#include <range/v3/core.hpp>
#include <range/v3/algorithm/lexicographical_compare.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"
#include "../test_iterators.hpp"

template<class Iter1, class Iter2, class Sent1 = Iter1, class Sent2 = Iter2>
void
test_iter1()
{
    int ia[] = {1, 2, 3, 4};
    constexpr auto sa = ranges::size(ia);
    int ib[] = {1, 2, 3};
    CHECK(!ranges::lexicographical_compare(Iter1(ia), Sent1(ia+sa), Iter2(ib), Sent2(ib+2)));
    CHECK(ranges::lexicographical_compare(Iter1(ib), Sent1(ib+2), Iter2(ia), Sent2(ia+sa)));
    CHECK(!ranges::lexicographical_compare(Iter1(ia), Sent1(ia+sa), Iter2(ib), Sent2(ib+3)));
    CHECK(ranges::lexicographical_compare(Iter1(ib), Sent1(ib+3), Iter2(ia), Sent2(ia+sa)));
    CHECK(ranges::lexicographical_compare(Iter1(ia), Sent1(ia+sa), Iter2(ib+1), Sent2(ib+3)));
    CHECK(!ranges::lexicographical_compare(Iter1(ib+1), Sent1(ib+3), Iter2(ia), Sent2(ia+sa)));
}

void test_iter()
{
    typedef sentinel<const int*> S;

    test_iter1<input_iterator<const int*>, input_iterator<const int*> >();
    test_iter1<input_iterator<const int*>, forward_iterator<const int*> >();
    test_iter1<input_iterator<const int*>, bidirectional_iterator<const int*> >();
    test_iter1<input_iterator<const int*>, random_access_iterator<const int*> >();
    test_iter1<input_iterator<const int*>, input_iterator<const int*>, S, S>();
    test_iter1<input_iterator<const int*>, forward_iterator<const int*>, S, S>();
    test_iter1<input_iterator<const int*>, bidirectional_iterator<const int*>, S, S>();
    test_iter1<input_iterator<const int*>, random_access_iterator<const int*>, S, S>();
    test_iter1<input_iterator<const int*>, const int*>();

    test_iter1<forward_iterator<const int*>, input_iterator<const int*> >();
    test_iter1<forward_iterator<const int*>, forward_iterator<const int*> >();
    test_iter1<forward_iterator<const int*>, bidirectional_iterator<const int*> >();
    test_iter1<forward_iterator<const int*>, random_access_iterator<const int*> >();
    test_iter1<forward_iterator<const int*>, input_iterator<const int*>, S, S>();
    test_iter1<forward_iterator<const int*>, forward_iterator<const int*>, S, S>();
    test_iter1<forward_iterator<const int*>, bidirectional_iterator<const int*>, S, S>();
    test_iter1<forward_iterator<const int*>, random_access_iterator<const int*>, S, S>();
    test_iter1<forward_iterator<const int*>, const int*>();

    test_iter1<bidirectional_iterator<const int*>, input_iterator<const int*> >();
    test_iter1<bidirectional_iterator<const int*>, forward_iterator<const int*> >();
    test_iter1<bidirectional_iterator<const int*>, bidirectional_iterator<const int*> >();
    test_iter1<bidirectional_iterator<const int*>, random_access_iterator<const int*> >();
    test_iter1<bidirectional_iterator<const int*>, input_iterator<const int*>, S, S>();
    test_iter1<bidirectional_iterator<const int*>, forward_iterator<const int*>, S, S>();
    test_iter1<bidirectional_iterator<const int*>, bidirectional_iterator<const int*>, S, S>();
    test_iter1<bidirectional_iterator<const int*>, random_access_iterator<const int*>, S, S>();
    test_iter1<bidirectional_iterator<const int*>, const int*>();

    test_iter1<random_access_iterator<const int*>, input_iterator<const int*> >();
    test_iter1<random_access_iterator<const int*>, forward_iterator<const int*> >();
    test_iter1<random_access_iterator<const int*>, bidirectional_iterator<const int*> >();
    test_iter1<random_access_iterator<const int*>, random_access_iterator<const int*> >();
    test_iter1<random_access_iterator<const int*>, input_iterator<const int*>, S, S>();
    test_iter1<random_access_iterator<const int*>, forward_iterator<const int*>, S, S>();
    test_iter1<random_access_iterator<const int*>, bidirectional_iterator<const int*>, S, S>();
    test_iter1<random_access_iterator<const int*>, random_access_iterator<const int*>, S, S>();
    test_iter1<random_access_iterator<const int*>, const int*>();

    test_iter1<const int*, input_iterator<const int*> >();
    test_iter1<const int*, forward_iterator<const int*> >();
    test_iter1<const int*, bidirectional_iterator<const int*> >();
    test_iter1<const int*, random_access_iterator<const int*> >();
    test_iter1<const int*, input_iterator<const int*>, const int*, S>();
    test_iter1<const int*, forward_iterator<const int*>, const int*, S>();
    test_iter1<const int*, bidirectional_iterator<const int*>, const int*, S>();
    test_iter1<const int*, random_access_iterator<const int*>, const int*, S>();
    test_iter1<const int*, const int*>();
}

template<class Iter1, class Iter2, class Sent1 = Iter1, class Sent2 = Iter2>
void
test_iter_comp1()
{
    int ia[] = {1, 2, 3, 4};
    const unsigned sa = sizeof(ia)/sizeof(ia[0]);
    int ib[] = {1, 2, 3};
    typedef std::greater<int> C;
    C c;
    CHECK(!ranges::lexicographical_compare(Iter1(ia), Sent1(ia+sa), Iter2(ib), Sent2(ib+2), c));
    CHECK(ranges::lexicographical_compare(Iter1(ib), Sent1(ib+2), Iter2(ia), Sent2(ia+sa), c));
    CHECK(!ranges::lexicographical_compare(Iter1(ia), Sent1(ia+sa), Iter2(ib), Sent2(ib+3), c));
    CHECK(ranges::lexicographical_compare(Iter1(ib), Sent1(ib+3), Iter2(ia), Sent2(ia+sa), c));
    CHECK(!ranges::lexicographical_compare(Iter1(ia), Sent1(ia+sa), Iter2(ib+1), Sent2(ib+3), c));
    CHECK(ranges::lexicographical_compare(Iter1(ib+1), Sent1(ib+3), Iter2(ia), Sent2(ia+sa), c));
}

void test_iter_comp()
{
    typedef sentinel<const int*> S;

    test_iter_comp1<input_iterator<const int*>, input_iterator<const int*> >();
    test_iter_comp1<input_iterator<const int*>, forward_iterator<const int*> >();
    test_iter_comp1<input_iterator<const int*>, bidirectional_iterator<const int*> >();
    test_iter_comp1<input_iterator<const int*>, random_access_iterator<const int*> >();
    test_iter_comp1<input_iterator<const int*>, input_iterator<const int*>, S, S>();
    test_iter_comp1<input_iterator<const int*>, forward_iterator<const int*>, S, S>();
    test_iter_comp1<input_iterator<const int*>, bidirectional_iterator<const int*>, S, S>();
    test_iter_comp1<input_iterator<const int*>, random_access_iterator<const int*>, S, S>();
    test_iter_comp1<input_iterator<const int*>, const int*>();

    test_iter_comp1<forward_iterator<const int*>, input_iterator<const int*> >();
    test_iter_comp1<forward_iterator<const int*>, forward_iterator<const int*> >();
    test_iter_comp1<forward_iterator<const int*>, bidirectional_iterator<const int*> >();
    test_iter_comp1<forward_iterator<const int*>, random_access_iterator<const int*> >();
    test_iter_comp1<forward_iterator<const int*>, input_iterator<const int*>, S, S>();
    test_iter_comp1<forward_iterator<const int*>, forward_iterator<const int*>, S, S>();
    test_iter_comp1<forward_iterator<const int*>, bidirectional_iterator<const int*>, S, S>();
    test_iter_comp1<forward_iterator<const int*>, random_access_iterator<const int*>, S, S>();
    test_iter_comp1<forward_iterator<const int*>, const int*>();

    test_iter_comp1<bidirectional_iterator<const int*>, input_iterator<const int*> >();
    test_iter_comp1<bidirectional_iterator<const int*>, forward_iterator<const int*> >();
    test_iter_comp1<bidirectional_iterator<const int*>, bidirectional_iterator<const int*> >();
    test_iter_comp1<bidirectional_iterator<const int*>, random_access_iterator<const int*> >();
    test_iter_comp1<bidirectional_iterator<const int*>, input_iterator<const int*>, S, S>();
    test_iter_comp1<bidirectional_iterator<const int*>, forward_iterator<const int*>, S, S>();
    test_iter_comp1<bidirectional_iterator<const int*>, bidirectional_iterator<const int*>, S, S>();
    test_iter_comp1<bidirectional_iterator<const int*>, random_access_iterator<const int*>, S, S>();
    test_iter_comp1<bidirectional_iterator<const int*>, const int*>();

    test_iter_comp1<random_access_iterator<const int*>, input_iterator<const int*> >();
    test_iter_comp1<random_access_iterator<const int*>, forward_iterator<const int*> >();
    test_iter_comp1<random_access_iterator<const int*>, bidirectional_iterator<const int*> >();
    test_iter_comp1<random_access_iterator<const int*>, random_access_iterator<const int*> >();
    test_iter_comp1<random_access_iterator<const int*>, input_iterator<const int*>, S, S>();
    test_iter_comp1<random_access_iterator<const int*>, forward_iterator<const int*>, S, S>();
    test_iter_comp1<random_access_iterator<const int*>, bidirectional_iterator<const int*>, S, S>();
    test_iter_comp1<random_access_iterator<const int*>, random_access_iterator<const int*>, S, S>();
    test_iter_comp1<random_access_iterator<const int*>, const int*>();

    test_iter_comp1<const int*, input_iterator<const int*> >();
    test_iter_comp1<const int*, forward_iterator<const int*> >();
    test_iter_comp1<const int*, bidirectional_iterator<const int*> >();
    test_iter_comp1<const int*, random_access_iterator<const int*> >();
    test_iter_comp1<const int*, input_iterator<const int*>, const int*, S>();
    test_iter_comp1<const int*, forward_iterator<const int*>, const int*, S>();
    test_iter_comp1<const int*, bidirectional_iterator<const int*>, const int*, S>();
    test_iter_comp1<const int*, random_access_iterator<const int*>, const int*, S>();
    test_iter_comp1<const int*, const int*>();
}


int main()
{
    test_iter();
    test_iter_comp();

    return test_result();
}
