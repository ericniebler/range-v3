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
//
//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include <functional>
#include <range/v3/core.hpp>
#include <range/v3/algorithm/set_algorithm.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"
#include "../test_iterators.hpp"

RANGES_DIAGNOSTIC_IGNORE_GLOBAL_CONSTRUCTORS

namespace
{
    auto const true_  = [](bool b){CHECK(b);};
    auto const false_ = [](bool b){CHECK(!b);};

    template<class Iter1, class Iter2>
    void
    test_iter()
    {
        int ia[] = {1, 2, 2, 3, 3, 3, 4, 4, 4, 4};
        const unsigned sa = sizeof(ia)/sizeof(ia[0]);
        int ib[] = {2, 4};
        const unsigned sb = sizeof(ib)/sizeof(ib[0]);
        int ic[] = {1, 2};
        int id[] = {3, 3, 3, 3};

        auto includes = make_testable_2<true, true>(ranges::includes);

        includes(Iter1(ia), Iter1(ia), Iter2(ib), Iter2(ib)).check(true_);
        includes(Iter1(ia), Iter1(ia), Iter2(ib), Iter2(ib+1)).check(false_);
        includes(Iter1(ia), Iter1(ia+1), Iter2(ib), Iter2(ib)).check(true_);
        includes(Iter1(ia), Iter1(ia+sa), Iter2(ia), Iter2(ia+sa)).check(true_);

        includes(Iter1(ia), Iter1(ia+sa), Iter2(ib), Iter2(ib+sb)).check(true_);
        includes(Iter1(ib), Iter1(ib+sb), Iter2(ia), Iter2(ia+sa)).check(false_);

        includes(Iter1(ia), Iter1(ia+2), Iter2(ic), Iter2(ic+2)).check(true_);
        includes(Iter1(ia), Iter1(ia+2), Iter2(ib), Iter2(ib+2)).check(false_);

        includes(Iter1(ia), Iter1(ia+sa), Iter2(id), Iter2(id+1)).check(true_);
        includes(Iter1(ia), Iter1(ia+sa), Iter2(id), Iter2(id+2)).check(true_);
        includes(Iter1(ia), Iter1(ia+sa), Iter2(id), Iter2(id+3)).check(true_);
        includes(Iter1(ia), Iter1(ia+sa), Iter2(id), Iter2(id+4)).check(false_);
    }

    template<class Iter1, class Iter2>
    void
    test_comp()
    {
        int ia[] = {1, 2, 2, 3, 3, 3, 4, 4, 4, 4};
        const unsigned sa = sizeof(ia)/sizeof(ia[0]);
        int ib[] = {2, 4};
        const unsigned sb = sizeof(ib)/sizeof(ib[0]);
        int ic[] = {1, 2};
        int id[] = {3, 3, 3, 3};

        auto includes = make_testable_2<true, true>(ranges::includes);

        includes(Iter1(ia), Iter1(ia), Iter2(ib), Iter2(ib), std::less<int>()).check(true_);
        includes(Iter1(ia), Iter1(ia), Iter2(ib), Iter2(ib+1), std::less<int>()).check(false_);
        includes(Iter1(ia), Iter1(ia+1), Iter2(ib), Iter2(ib), std::less<int>()).check(true_);
        includes(Iter1(ia), Iter1(ia+sa), Iter2(ia), Iter2(ia+sa), std::less<int>()).check(true_);

        includes(Iter1(ia), Iter1(ia+sa), Iter2(ib), Iter2(ib+sb), std::less<int>()).check(true_);
        includes(Iter1(ib), Iter1(ib+sb), Iter2(ia), Iter2(ia+sa), std::less<int>()).check(false_);

        includes(Iter1(ia), Iter1(ia+2), Iter2(ic), Iter2(ic+2), std::less<int>()).check(true_);
        includes(Iter1(ia), Iter1(ia+2), Iter2(ib), Iter2(ib+2), std::less<int>()).check(false_);

        includes(Iter1(ia), Iter1(ia+sa), Iter2(id), Iter2(id+1), std::less<int>()).check(true_);
        includes(Iter1(ia), Iter1(ia+sa), Iter2(id), Iter2(id+2), std::less<int>()).check(true_);
        includes(Iter1(ia), Iter1(ia+sa), Iter2(id), Iter2(id+3), std::less<int>()).check(true_);
        includes(Iter1(ia), Iter1(ia+sa), Iter2(id), Iter2(id+4), std::less<int>()).check(false_);
    }

    template<class Iter1, class Iter2>
    void test()
    {
        test_iter<Iter1, Iter2>();
        test_comp<Iter1, Iter2>();
    }

    struct S
    {
        int i;
    };

    struct T
    {
        int j;
    };
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

    // Test projections
    {
        S ia[] = {{1}, {2}, {2}, {3}, {3}, {3}, {4}, {4}, {4}, {4}};
        T id[] = {{3}, {3}, {3}};
        CHECK(ranges::includes(ia, id, std::less<int>(), &S::i, &T::j));
    }

    // Test initializer list
    {
        CHECK(ranges::includes(
            {S{1}, S{2}, S{2}, S{3}, S{3}, S{3}, S{4}, S{4}, S{4}, S{4}},
            {T{3}, T{3}, T{3}},
            std::less<int>(),
            &S::i, &T::j
        ));
    }

    return ::test_result();
}
