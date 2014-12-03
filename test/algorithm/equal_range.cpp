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

int main()
{
    int d[] = {0, 1, 2, 3};
    for (int* e = d; e <= d+4; ++e)
        for (int x = -1; x <= 4; ++x)
            test(d, e, x);

    test<forward_iterator<const int*> >();
    test<bidirectional_iterator<const int*> >();
    test<random_access_iterator<const int*> >();
    test<const int*>();

    test<forward_iterator<const int*>, sentinel<const int*> >();
    test<bidirectional_iterator<const int*>, sentinel<const int*> >();
    test<random_access_iterator<const int*>, sentinel<const int*> >();

    return ::test_result();
}
