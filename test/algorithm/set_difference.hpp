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

#include <algorithm>
#include <functional>
#include <range/v3/core.hpp>
#include <range/v3/algorithm/fill.hpp>
#include <range/v3/algorithm/set_algorithm.hpp>
#include <range/v3/algorithm/lexicographical_compare.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"
#include "../test_iterators.hpp"

template<class Iter1, class Iter2, class OutIter>
void
test_iter()
{
    int ia[] = {1, 2, 2, 3, 3, 3, 4, 4, 4, 4};
    static const int sa = sizeof(ia)/sizeof(ia[0]);
    int ib[] = {2, 4, 4, 6};
    static const int sb = sizeof(ib)/sizeof(ib[0]);
    int ic[20];
    int ir[] = {1, 2, 3, 3, 3, 4, 4};
    static const int sr = sizeof(ir)/sizeof(ir[0]);

    auto set_difference = ::make_testable_2<false, true>(ranges::set_difference);

    set_difference(Iter1(ia), Iter1(ia+sa), Iter2(ib), Iter2(ib+sb), OutIter(ic)).
        check([&](std::pair<Iter1, OutIter> res)
        {
            CHECK((base(res.first) - ia) == sa);
            CHECK((base(res.second) - ic) == sr);
            CHECK(std::lexicographical_compare(ic, base(res.second), ir, ir+sr) == false);
            ranges::fill(ic, 0);
        }
    );
    int irr[] = {6};
    static const int srr = sizeof(irr)/sizeof(irr[0]);
    set_difference(Iter1(ib), Iter1(ib+sb), Iter2(ia), Iter2(ia+sa), OutIter(ic)).
        check([&](std::pair<Iter1, OutIter> res)
        {
            CHECK((base(res.first) - ib) == sb);
            CHECK((base(res.second) - ic) == srr);
            CHECK(std::lexicographical_compare(ic, base(res.second), irr, irr+srr) == false);
            ranges::fill(ic, 0);
        }
    );
}

template<class Iter1, class Iter2, class OutIter>
void
test_comp()
{
    int ia[] = {1, 2, 2, 3, 3, 3, 4, 4, 4, 4};
    static const int sa = sizeof(ia)/sizeof(ia[0]);
    int ib[] = {2, 4, 4, 6};
    static const int sb = sizeof(ib)/sizeof(ib[0]);
    int ic[20];
    int ir[] = {1, 2, 3, 3, 3, 4, 4};
    static const int sr = sizeof(ir)/sizeof(ir[0]);

    auto set_difference = ::make_testable_2<false, true>(ranges::set_difference);

    set_difference(Iter1(ia), Iter1(ia+sa), Iter2(ib), Iter2(ib+sb), OutIter(ic), std::less<int>()).
        check([&](std::pair<Iter1, OutIter> res)
        {
            CHECK((base(res.first) - ia) == sa);
            CHECK((base(res.second) - ic) == sr);
            CHECK(std::lexicographical_compare(ic, base(res.second), ir, ir+sr) == false);
            ranges::fill(ic, 0);
        }
    );
    int irr[] = {6};
    static const int srr = sizeof(irr)/sizeof(irr[0]);
    set_difference(Iter1(ib), Iter1(ib+sb), Iter2(ia), Iter2(ia+sa), OutIter(ic), std::less<int>()).
        check([&](std::pair<Iter1, OutIter> res)
        {
            CHECK((base(res.first) - ib) == sb);
            CHECK((base(res.second) - ic) == srr);
            CHECK(std::lexicographical_compare(ic, base(res.second), irr, irr+srr) == false);
            ranges::fill(ic, 0);
        }
    );
}

template<class Iter1, class Iter2, class OutIter>
void test()
{
    test_iter<Iter1, Iter2, OutIter>();
    test_comp<Iter1, Iter2, OutIter>();
}

struct S
{
    int i;
};

struct T
{
    int j;
};

struct U
{
    int k;
    U& operator=(S s) { k = s.i; return *this;}
    U& operator=(T t) { k = t.j; return *this;}
};

int main()
{
#ifdef SET_DIFFERENCE_1
    test<input_iterator<const int*>, input_iterator<const int*>, output_iterator<int*> >();
    test<input_iterator<const int*>, input_iterator<const int*>, forward_iterator<int*> >();
    test<input_iterator<const int*>, input_iterator<const int*>, bidirectional_iterator<int*> >();
    test<input_iterator<const int*>, input_iterator<const int*>, random_access_iterator<int*> >();
    test<input_iterator<const int*>, input_iterator<const int*>, int*>();

    test<input_iterator<const int*>, forward_iterator<const int*>, output_iterator<int*> >();
    test<input_iterator<const int*>, forward_iterator<const int*>, forward_iterator<int*> >();
    test<input_iterator<const int*>, forward_iterator<const int*>, bidirectional_iterator<int*> >();
    test<input_iterator<const int*>, forward_iterator<const int*>, random_access_iterator<int*> >();
    test<input_iterator<const int*>, forward_iterator<const int*>, int*>();

    test<input_iterator<const int*>, bidirectional_iterator<const int*>, output_iterator<int*> >();
    test<input_iterator<const int*>, bidirectional_iterator<const int*>, forward_iterator<int*> >();
    test<input_iterator<const int*>, bidirectional_iterator<const int*>, bidirectional_iterator<int*> >();
    test<input_iterator<const int*>, bidirectional_iterator<const int*>, random_access_iterator<int*> >();
    test<input_iterator<const int*>, bidirectional_iterator<const int*>, int*>();

    test<input_iterator<const int*>, random_access_iterator<const int*>, output_iterator<int*> >();
    test<input_iterator<const int*>, random_access_iterator<const int*>, forward_iterator<int*> >();
    test<input_iterator<const int*>, random_access_iterator<const int*>, bidirectional_iterator<int*> >();
    test<input_iterator<const int*>, random_access_iterator<const int*>, random_access_iterator<int*> >();
    test<input_iterator<const int*>, random_access_iterator<const int*>, int*>();

    test<input_iterator<const int*>, const int*, output_iterator<int*> >();
    test<input_iterator<const int*>, const int*, forward_iterator<int*> >();
    test<input_iterator<const int*>, const int*, bidirectional_iterator<int*> >();
    test<input_iterator<const int*>, const int*, random_access_iterator<int*> >();
    test<input_iterator<const int*>, const int*, int*>();
#endif
#ifdef SET_DIFFERENCE_2
    test<forward_iterator<const int*>, input_iterator<const int*>, output_iterator<int*> >();
    test<forward_iterator<const int*>, input_iterator<const int*>, forward_iterator<int*> >();
    test<forward_iterator<const int*>, input_iterator<const int*>, bidirectional_iterator<int*> >();
    test<forward_iterator<const int*>, input_iterator<const int*>, random_access_iterator<int*> >();
    test<forward_iterator<const int*>, input_iterator<const int*>, int*>();

    test<forward_iterator<const int*>, forward_iterator<const int*>, output_iterator<int*> >();
    test<forward_iterator<const int*>, forward_iterator<const int*>, forward_iterator<int*> >();
    test<forward_iterator<const int*>, forward_iterator<const int*>, bidirectional_iterator<int*> >();
    test<forward_iterator<const int*>, forward_iterator<const int*>, random_access_iterator<int*> >();
    test<forward_iterator<const int*>, forward_iterator<const int*>, int*>();

    test<forward_iterator<const int*>, bidirectional_iterator<const int*>, output_iterator<int*> >();
    test<forward_iterator<const int*>, bidirectional_iterator<const int*>, forward_iterator<int*> >();
    test<forward_iterator<const int*>, bidirectional_iterator<const int*>, bidirectional_iterator<int*> >();
    test<forward_iterator<const int*>, bidirectional_iterator<const int*>, random_access_iterator<int*> >();
    test<forward_iterator<const int*>, bidirectional_iterator<const int*>, int*>();

    test<forward_iterator<const int*>, random_access_iterator<const int*>, output_iterator<int*> >();
    test<forward_iterator<const int*>, random_access_iterator<const int*>, forward_iterator<int*> >();
    test<forward_iterator<const int*>, random_access_iterator<const int*>, bidirectional_iterator<int*> >();
    test<forward_iterator<const int*>, random_access_iterator<const int*>, random_access_iterator<int*> >();
    test<forward_iterator<const int*>, random_access_iterator<const int*>, int*>();

    test<forward_iterator<const int*>, const int*, output_iterator<int*> >();
    test<forward_iterator<const int*>, const int*, forward_iterator<int*> >();
    test<forward_iterator<const int*>, const int*, bidirectional_iterator<int*> >();
    test<forward_iterator<const int*>, const int*, random_access_iterator<int*> >();
    test<forward_iterator<const int*>, const int*, int*>();
#endif
#ifdef SET_DIFFERENCE_3
    test<bidirectional_iterator<const int*>, input_iterator<const int*>, output_iterator<int*> >();
    test<bidirectional_iterator<const int*>, input_iterator<const int*>, forward_iterator<int*> >();
    test<bidirectional_iterator<const int*>, input_iterator<const int*>, bidirectional_iterator<int*> >();
    test<bidirectional_iterator<const int*>, input_iterator<const int*>, random_access_iterator<int*> >();
    test<bidirectional_iterator<const int*>, input_iterator<const int*>, int*>();

    test<bidirectional_iterator<const int*>, forward_iterator<const int*>, output_iterator<int*> >();
    test<bidirectional_iterator<const int*>, forward_iterator<const int*>, forward_iterator<int*> >();
    test<bidirectional_iterator<const int*>, forward_iterator<const int*>, bidirectional_iterator<int*> >();
    test<bidirectional_iterator<const int*>, forward_iterator<const int*>, random_access_iterator<int*> >();
    test<bidirectional_iterator<const int*>, forward_iterator<const int*>, int*>();

    test<bidirectional_iterator<const int*>, bidirectional_iterator<const int*>, output_iterator<int*> >();
    test<bidirectional_iterator<const int*>, bidirectional_iterator<const int*>, forward_iterator<int*> >();
    test<bidirectional_iterator<const int*>, bidirectional_iterator<const int*>, bidirectional_iterator<int*> >();
    test<bidirectional_iterator<const int*>, bidirectional_iterator<const int*>, random_access_iterator<int*> >();
    test<bidirectional_iterator<const int*>, bidirectional_iterator<const int*>, int*>();

    test<bidirectional_iterator<const int*>, random_access_iterator<const int*>, output_iterator<int*> >();
    test<bidirectional_iterator<const int*>, random_access_iterator<const int*>, forward_iterator<int*> >();
    test<bidirectional_iterator<const int*>, random_access_iterator<const int*>, bidirectional_iterator<int*> >();
    test<bidirectional_iterator<const int*>, random_access_iterator<const int*>, random_access_iterator<int*> >();
    test<bidirectional_iterator<const int*>, random_access_iterator<const int*>, int*>();

    test<bidirectional_iterator<const int*>, const int*, output_iterator<int*> >();
    test<bidirectional_iterator<const int*>, const int*, forward_iterator<int*> >();
    test<bidirectional_iterator<const int*>, const int*, bidirectional_iterator<int*> >();
    test<bidirectional_iterator<const int*>, const int*, random_access_iterator<int*> >();
    test<bidirectional_iterator<const int*>, const int*, int*>();
#endif
#ifdef SET_DIFFERENCE_4
    test<random_access_iterator<const int*>, input_iterator<const int*>, output_iterator<int*> >();
    test<random_access_iterator<const int*>, input_iterator<const int*>, forward_iterator<int*> >();
    test<random_access_iterator<const int*>, input_iterator<const int*>, bidirectional_iterator<int*> >();
    test<random_access_iterator<const int*>, input_iterator<const int*>, random_access_iterator<int*> >();
    test<random_access_iterator<const int*>, input_iterator<const int*>, int*>();

    test<random_access_iterator<const int*>, forward_iterator<const int*>, output_iterator<int*> >();
    test<random_access_iterator<const int*>, forward_iterator<const int*>, forward_iterator<int*> >();
    test<random_access_iterator<const int*>, forward_iterator<const int*>, bidirectional_iterator<int*> >();
    test<random_access_iterator<const int*>, forward_iterator<const int*>, random_access_iterator<int*> >();
    test<random_access_iterator<const int*>, forward_iterator<const int*>, int*>();

    test<random_access_iterator<const int*>, bidirectional_iterator<const int*>, output_iterator<int*> >();
    test<random_access_iterator<const int*>, bidirectional_iterator<const int*>, forward_iterator<int*> >();
    test<random_access_iterator<const int*>, bidirectional_iterator<const int*>, bidirectional_iterator<int*> >();
    test<random_access_iterator<const int*>, bidirectional_iterator<const int*>, random_access_iterator<int*> >();
    test<random_access_iterator<const int*>, bidirectional_iterator<const int*>, int*>();

    test<random_access_iterator<const int*>, random_access_iterator<const int*>, output_iterator<int*> >();
    test<random_access_iterator<const int*>, random_access_iterator<const int*>, forward_iterator<int*> >();
    test<random_access_iterator<const int*>, random_access_iterator<const int*>, bidirectional_iterator<int*> >();
    test<random_access_iterator<const int*>, random_access_iterator<const int*>, random_access_iterator<int*> >();
    test<random_access_iterator<const int*>, random_access_iterator<const int*>, int*>();

    test<random_access_iterator<const int*>, const int*, output_iterator<int*> >();
    test<random_access_iterator<const int*>, const int*, forward_iterator<int*> >();
    test<random_access_iterator<const int*>, const int*, bidirectional_iterator<int*> >();
    test<random_access_iterator<const int*>, const int*, random_access_iterator<int*> >();
    test<random_access_iterator<const int*>, const int*, int*>();
#endif
#ifdef SET_DIFFERENCE_5
    test<const int*, input_iterator<const int*>, output_iterator<int*> >();
    test<const int*, input_iterator<const int*>, bidirectional_iterator<int*> >();    test<const int*, input_iterator<const int*>, bidirectional_iterator<int*> >();
    test<const int*, input_iterator<const int*>, random_access_iterator<int*> >();
    test<const int*, input_iterator<const int*>, int*>();

    test<const int*, forward_iterator<const int*>, output_iterator<int*> >();
    test<const int*, forward_iterator<const int*>, forward_iterator<int*> >();
    test<const int*, forward_iterator<const int*>, bidirectional_iterator<int*> >();
    test<const int*, forward_iterator<const int*>, random_access_iterator<int*> >();
    test<const int*, forward_iterator<const int*>, int*>();

    test<const int*, bidirectional_iterator<const int*>, output_iterator<int*> >();
    test<const int*, bidirectional_iterator<const int*>, forward_iterator<int*> >();
    test<const int*, bidirectional_iterator<const int*>, bidirectional_iterator<int*> >();
    test<const int*, bidirectional_iterator<const int*>, random_access_iterator<int*> >();
    test<const int*, bidirectional_iterator<const int*>, int*>();

    test<const int*, random_access_iterator<const int*>, output_iterator<int*> >();
    test<const int*, random_access_iterator<const int*>, forward_iterator<int*> >();
    test<const int*, random_access_iterator<const int*>, bidirectional_iterator<int*> >();
    test<const int*, random_access_iterator<const int*>, random_access_iterator<int*> >();
    test<const int*, random_access_iterator<const int*>, int*>();

    test<const int*, const int*, output_iterator<int*> >();
    test<const int*, const int*, forward_iterator<int*> >();
    test<const int*, const int*, bidirectional_iterator<int*> >();
    test<const int*, const int*, random_access_iterator<int*> >();
    test<const int*, const int*, int*>();
#endif
#ifdef SET_DIFFERENCE_6
    // Test projections
    {
        S ia[] = {S{1}, S{2}, S{2}, S{3}, S{3}, S{3}, S{4}, S{4}, S{4}, S{4}};
        static const int sa = sizeof(ia)/sizeof(ia[0]);
        T ib[] = {T{2}, T{4}, T{4}, T{6}};
        static const int sb = sizeof(ib)/sizeof(ib[0]);
        U ic[20];
        int ir[] = {1, 2, 3, 3, 3, 4, 4};
        static const int sr = sizeof(ir)/sizeof(ir[0]);

        std::pair<S *, U *> res = ranges::set_difference(ia, ib, ic, std::less<int>(), &S::i, &T::j);
        CHECK((res.first - ia) == sa);
        CHECK((res.second - ic) == sr);
        CHECK(ranges::lexicographical_compare(ic, res.second, ir, ir+sr, std::less<int>(), &U::k) == false);
        ranges::fill(ic, U{0});

        int irr[] = {6};
        static const int srr = sizeof(irr)/sizeof(irr[0]);
        std::pair<T *, U *> res2 = ranges::set_difference(ib, ia, ic, std::less<int>(), &T::j, &S::i);
        CHECK((res2.first - ib) == sb);
        CHECK((res2.second - ic) == srr);
        CHECK(ranges::lexicographical_compare(ic, res2.second, ir, irr+srr, std::less<int>(), &U::k) == false);
    }

    // Test rvalue ranges
    {
        S ia[] = {S{1}, S{2}, S{2}, S{3}, S{3}, S{3}, S{4}, S{4}, S{4}, S{4}};
        static const int sa = sizeof(ia)/sizeof(ia[0]);
        T ib[] = {T{2}, T{4}, T{4}, T{6}};
        static const int sb = sizeof(ib)/sizeof(ib[0]);
        U ic[20];
        int ir[] = {1, 2, 3, 3, 3, 4, 4};
        static const int sr = sizeof(ir)/sizeof(ir[0]);

        auto res = ranges::set_difference(ranges::view::all(ia), ranges::view::all(ib), ic, std::less<int>(), &S::i, &T::j);
        CHECK((res.first.get_unsafe() - ia) == sa);
        CHECK((res.second - ic) == sr);
        CHECK(ranges::lexicographical_compare(ic, res.second, ir, ir+sr, std::less<int>(), &U::k) == false);
        ranges::fill(ic, U{0});

        int irr[] = {6};
        static const int srr = sizeof(irr)/sizeof(irr[0]);
        auto res2 = ranges::set_difference(ranges::view::all(ib), ranges::view::all(ia), ic, std::less<int>(), &T::j, &S::i);
        CHECK((res2.first.get_unsafe() - ib) == sb);
        CHECK((res2.second - ic) == srr);
        CHECK(ranges::lexicographical_compare(ic, res2.second, ir, irr+srr, std::less<int>(), &U::k) == false);
    }

    // Test initializer list
    {
        S ia[] = {S{1}, S{2}, S{2}, S{3}, S{3}, S{3}, S{4}, S{4}, S{4}, S{4}};
        static const int sa = sizeof(ia)/sizeof(ia[0]);
        T ib[] = {T{2}, T{4}, T{4}, T{6}};
        static const int sb = sizeof(ib)/sizeof(ib[0]);
        U ic[20];
        int ir[] = {1, 2, 3, 3, 3, 4, 4};
        static const int sr = sizeof(ir)/sizeof(ir[0]);

        std::pair<S *, U *> res = ranges::set_difference(
            ia,
            {T{2}, T{4}, T{4}, T{6}},
            ic,
            std::less<int>(), &S::i, &T::j);
        CHECK((res.first - ia) == sa);
        CHECK((res.second - ic) == sr);
        CHECK(ranges::lexicographical_compare(ic, res.second, ir, ir+sr, std::less<int>(), &U::k) == false);
        ranges::fill(ic, U{0});

        int irr[] = {6};
        static const int srr = sizeof(irr)/sizeof(irr[0]);
        std::pair<T *, U *> res2 = ranges::set_difference(
            ib,
            {S{1}, S{2}, S{2}, S{3}, S{3}, S{3}, S{4}, S{4}, S{4}, S{4}},
            ic,
            std::less<int>(), &T::j, &S::i);
        CHECK((res2.first - ib) == sb);
        CHECK((res2.second - ic) == srr);
        CHECK(ranges::lexicographical_compare(ic, res2.second, ir, irr+srr, std::less<int>(), &U::k) == false);
    }
#endif

    return ::test_result();
}
