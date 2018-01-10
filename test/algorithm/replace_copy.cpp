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

#include <utility>
#include <range/v3/core.hpp>
#include <range/v3/algorithm/replace_copy.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"
#include "../test_iterators.hpp"

template<class InIter, class OutIter, class Sent = InIter>
void test_iter()
{
    int ia[] = {0, 1, 2, 3, 4};
    const unsigned sa = sizeof(ia)/sizeof(ia[0]);
    int ib[sa] = {0};
    std::pair<InIter, OutIter> r = ranges::replace_copy(InIter(ia), Sent(ia+sa), OutIter(ib), 2, 5);
    CHECK(base(r.first) == ia + sa);
    CHECK(base(r.second) == ib + sa);
    CHECK(ib[0] == 0);
    CHECK(ib[1] == 1);
    CHECK(ib[2] == 5);
    CHECK(ib[3] == 3);
    CHECK(ib[4] == 4);
}

template<class InIter, class OutIter, class Sent = InIter>
void test_rng()
{
    int ia[] = {0, 1, 2, 3, 4};
    const unsigned sa = sizeof(ia)/sizeof(ia[0]);
    int ib[sa] = {0};
    auto rng = ranges::make_iterator_range(InIter(ia), Sent(ia+sa));
    std::pair<InIter, OutIter> r = ranges::replace_copy(rng, OutIter(ib), 2, 5);
    CHECK(base(r.first) == ia + sa);
    CHECK(base(r.second) == ib + sa);
    CHECK(ib[0] == 0);
    CHECK(ib[1] == 1);
    CHECK(ib[2] == 5);
    CHECK(ib[3] == 3);
    CHECK(ib[4] == 4);
}

template<class InIter, class OutIter>
void test()
{
    using Sent = typename sentinel_type<InIter>::type;
    test_iter<InIter, OutIter>();
    test_iter<InIter, OutIter>();
    test_rng<InIter, OutIter, Sent>();
    test_rng<InIter, OutIter, Sent>();
}

int main()
{
    test<input_iterator<const int*>, output_iterator<int*> >();
    test<input_iterator<const int*>, forward_iterator<int*> >();
    test<input_iterator<const int*>, bidirectional_iterator<int*> >();
    test<input_iterator<const int*>, random_access_iterator<int*> >();
    test<input_iterator<const int*>, int*>();

    test<forward_iterator<const int*>, output_iterator<int*> >();
    test<forward_iterator<const int*>, forward_iterator<int*> >();
    test<forward_iterator<const int*>, bidirectional_iterator<int*> >();
    test<forward_iterator<const int*>, random_access_iterator<int*> >();
    test<forward_iterator<const int*>, int*>();

    test<bidirectional_iterator<const int*>, output_iterator<int*> >();
    test<bidirectional_iterator<const int*>, forward_iterator<int*> >();
    test<bidirectional_iterator<const int*>, bidirectional_iterator<int*> >();
    test<bidirectional_iterator<const int*>, random_access_iterator<int*> >();
    test<bidirectional_iterator<const int*>, int*>();

    test<random_access_iterator<const int*>, output_iterator<int*> >();
    test<random_access_iterator<const int*>, forward_iterator<int*> >();
    test<random_access_iterator<const int*>, bidirectional_iterator<int*> >();
    test<random_access_iterator<const int*>, random_access_iterator<int*> >();
    test<random_access_iterator<const int*>, int*>();

    test<const int*, output_iterator<int*> >();
    test<const int*, forward_iterator<int*> >();
    test<const int*, bidirectional_iterator<int*> >();
    test<const int*, random_access_iterator<int*> >();
    test<const int*, int*>();

    // Test projection
    {
        using P = std::pair<int, std::string>;
        P in[] = {{0, "0"}, {1, "1"}, {2, "2"}, {3, "3"}, {4, "4"}};
        P out[ranges::size(in)] = {};
        std::pair<P *, P *> r = ranges::replace_copy(in, out, 2, P{5, "5"}, &std::pair<int, std::string>::first);
        CHECK(r.first == ranges::end(in));
        CHECK(r.second == ranges::end(out));
        CHECK(out[0] == P{0, "0"});
        CHECK(out[1] == P{1, "1"});
        CHECK(out[2] == P{5, "5"});
        CHECK(out[3] == P{3, "3"});
        CHECK(out[4] == P{4, "4"});
    }

    return ::test_result();
}
