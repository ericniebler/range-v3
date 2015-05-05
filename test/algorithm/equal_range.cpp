// Range v3 library
//
//  Copyright Eric Niebler 2014
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

#include <vector>
#include <iterator>
#include <range/v3/core.hpp>
#include <range/v3/view/join.hpp>
#include <range/v3/view/transform.hpp>
#include <range/v3/view/iota.hpp>
#include <range/v3/view/repeat_n.hpp>
#include <range/v3/view/take.hpp>
#include <range/v3/utility/iterator.hpp>
#include "../array.hpp"
#include <range/v3/algorithm/copy.hpp>
#include <range/v3/algorithm/equal_range.hpp>
#include "../simple_test.hpp"
#include "../test_iterators.hpp"

template <class Iter, class Sent, class T>
void
test(Iter first, Sent last, const T& value)
{
    ranges::range<Iter, Iter> i = ranges::equal_range(first, last, value);
    for (Iter j = first; j != i.begin(); ++j)
        CHECK(*j < value);
    for (Iter j = i.begin(); j != last; ++j)
        CHECK(!(*j < value));
    for (Iter j = first; j != i.end(); ++j)
        CHECK(!(value < *j));
    for (Iter j = i.end(); j != last; ++j)
        CHECK(value < *j);

    auto res = ranges::equal_range(ranges::make_range(first, last), value);
    for (Iter j = first; j != res.get_unsafe().begin(); ++j)
        CHECK(*j < value);
    for (Iter j = res.get_unsafe().begin(); j != last; ++j)
        CHECK(!(*j < value));
    for (Iter j = first; j != res.get_unsafe().end(); ++j)
        CHECK(!(value < *j));
    for (Iter j = res.get_unsafe().end(); j != last; ++j)
        CHECK(value < *j);
}

template <class Iter, class Sent = Iter>
void
test()
{
    using namespace ranges::view;
    static constexpr unsigned M = 10;
    std::vector<int> v;
    auto input = ints | take(100) | transform([](int i){return repeat_n(i,M);}) | join;
    ranges::copy(input, ranges::back_inserter(v));
    for (int x = 0; x <= (int)M; ++x)
        test(Iter(v.data()), Sent(v.data()+v.size()), x);
}

#ifdef RANGES_CXX_GREATER_THAN_11

template <class Iter, class Sent, class T>
RANGES_CXX14_CONSTEXPR bool
test_constexpr(Iter first, Sent last, const T& value)
{
    bool result = true;
    ranges::range<Iter, Iter> i = ranges::equal_range(first, last, value);
    for (Iter j = first; j != i.begin(); ++j)
        if(!(*j < value)) { result = false; }
    for (Iter j = i.begin(); j != last; ++j)
        if(!(!(*j < value))) { result = false; }
    for (Iter j = first; j != i.end(); ++j)
        if(!(!(value < *j))) { result = false; }
    for (Iter j = i.end(); j != last; ++j)
        if(!(value < *j)) { result = false; }

    auto res = ranges::equal_range(ranges::make_range(first, last), value);
    for (Iter j = first; j != res.get_unsafe().begin(); ++j)
        if(!(*j < value)) { result = false; }
    for (Iter j = res.get_unsafe().begin(); j != last; ++j)
        if(!(!(*j < value))) { result = false; }
    for (Iter j = first; j != res.get_unsafe().end(); ++j)
        if(!(!(value < *j))) { result = false; }
    for (Iter j = res.get_unsafe().end(); j != last; ++j)
        if(!(value < *j)) { result = false; }

    return result;
}


// TODO: constexpr
// need to make views constexpr before adding this test
// template <class Iter, class Sent = Iter>
// RANGES_CXX14_CONSTEXPR bool
// test_constexpr()
// {
//     using namespace ranges::view;
//     constexpr unsigned M = 10;
//     constexpr unsigned N = 10;
//     array<int, N * M> v{{0}};
//     auto input = ints | take(N)
//                  | transform(transform_f{M}) | join;
//     ranges::copy(input, ranges::begin(v));
//     bool result = true;
//     for (int x = 0; x <= (int)M; ++x)
//         if(!test_constexpr(Iter(v.data()), Sent(v.data()+v.size()), x)) {
//             result = false;
//         }
//     return result;
// }

RANGES_CXX14_CONSTEXPR bool
test_constexpr_some()
{
    int d[] = {0, 1, 2, 3};
    int* end = d+4;
    bool result = true;
    for (int* e = d; e < end; ++e)
        for (int x = -1; x <= 4; ++x)
            if(!test_constexpr(d, e, x)) { result = false; };
    return result;
}

#endif

int main()
{
    test<forward_iterator<const int*> >();
    test<bidirectional_iterator<const int*> >();
    test<random_access_iterator<const int*> >();
    test<const int*>();

    test<forward_iterator<const int*>, sentinel<const int*> >();
    test<bidirectional_iterator<const int*>, sentinel<const int*> >();
    test<random_access_iterator<const int*>, sentinel<const int*> >();

#ifdef RANGES_CXX_GREATER_THAN_11
    {
        static_assert(test_constexpr_some(), "");
        // TODO: constexpr
        // need to make views constexpr before adding these tests
        // static_assert(test_constexpr<forward_iterator<const int*> >(), "");
        // static_assert(test_constexpr<bidirectional_iterator<const int*> >(), "");
        // static_assert(test_constexpr<random_access_iterator<const int*> >(), "");
        // static_assert(test_constexpr<const int*>(), "");
        // static_assert(test_constexpr<forward_iterator<const int*>, sentinel<const int*> >(), "");
        // static_assert(test_constexpr<bidirectional_iterator<const int*>, sentinel<const int*> >(), "");
        // static_assert(test_constexpr<random_access_iterator<const int*>, sentinel<const int*> >(), "");
    }
#endif

    return ::test_result();
}
