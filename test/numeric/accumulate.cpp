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

#include <range/v3/core.hpp>
#include <range/v3/numeric/accumulate.hpp>
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

template<class Iter, class Sent = Iter>
void test()
{
    int ia[] = {1, 2, 3, 4, 5, 6};
    constexpr auto sc = ranges::size(ia);
    CHECK(ranges::accumulate(Iter(ia), Sent(ia), 0) == 0);
    CHECK(ranges::accumulate(Iter(ia), Sent(ia), 10) == 10);
    CHECK(ranges::accumulate(Iter(ia), Sent(ia+1), 0) == 1);
    CHECK(ranges::accumulate(Iter(ia), Sent(ia+1), 10) == 11);
    CHECK(ranges::accumulate(Iter(ia), Sent(ia+2), 0) == 3);
    CHECK(ranges::accumulate(Iter(ia), Sent(ia+2), 10) == 13);
    CHECK(ranges::accumulate(Iter(ia), Sent(ia+sc), 0) == 21);
    CHECK(ranges::accumulate(Iter(ia), Sent(ia+sc), 10) == 31);

    using ranges::make_iterator_range;
    CHECK(ranges::accumulate(make_iterator_range(Iter(ia), Sent(ia)), 0) == 0);
    CHECK(ranges::accumulate(make_iterator_range(Iter(ia), Sent(ia)), 10) == 10);
    CHECK(ranges::accumulate(make_iterator_range(Iter(ia), Sent(ia+1)), 0) == 1);
    CHECK(ranges::accumulate(make_iterator_range(Iter(ia), Sent(ia+1)), 10) == 11);
    CHECK(ranges::accumulate(make_iterator_range(Iter(ia), Sent(ia+2)), 0) == 3);
    CHECK(ranges::accumulate(make_iterator_range(Iter(ia), Sent(ia+2)), 10) == 13);
    CHECK(ranges::accumulate(make_iterator_range(Iter(ia), Sent(ia+sc)), 0) == 21);
    CHECK(ranges::accumulate(make_iterator_range(Iter(ia), Sent(ia+sc)), 10) == 31);
}

int main()
{
    test<input_iterator<const int*> >();
    test<forward_iterator<const int*> >();
    test<bidirectional_iterator<const int*> >();
    test<random_access_iterator<const int*> >();
    test<const int*>();

    test<input_iterator<const int*>, sentinel<const int*> >();
    test<forward_iterator<const int*>, sentinel<const int*> >();
    test<bidirectional_iterator<const int*>, sentinel<const int*> >();
    test<random_access_iterator<const int*>, sentinel<const int*> >();

    CHECK(ranges::accumulate({1, 2, 3, 4, 5, 6}, 10) == 31);
    CHECK(ranges::accumulate({1, 2, 3, 4, 5, 6}, S{10}, &S::add).i == 31);
    CHECK(ranges::accumulate({S{1}, S{2}, S{3}, S{4}, S{5}, S{6}}, 10, ranges::plus{}, &S::i) == 31);

    return ::test_result();
}
