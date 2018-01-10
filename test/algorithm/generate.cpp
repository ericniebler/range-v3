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

#include <vector>
#include <range/v3/core.hpp>
#include <range/v3/algorithm/generate.hpp>
#include <range/v3/view/counted.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"
#include "../test_iterators.hpp"

struct gen_test
{
    int i_;
    gen_test() = default;
    gen_test(int i) : i_(i) {}
    int operator()() {return i_++;}
};

template<class Iter, class Sent = Iter>
void
test()
{
    const unsigned n = 4;
    int ia[n] = {0};
    std::pair<Iter, gen_test> res = ranges::generate(Iter(ia), Sent(ia + n), gen_test(1));
    CHECK(ia[0] == 1);
    CHECK(ia[1] == 2);
    CHECK(ia[2] == 3);
    CHECK(ia[3] == 4);
    CHECK(res.first == Iter(ia + n));
    CHECK(res.second.i_ == 5);

    auto rng = ranges::make_iterator_range(Iter(ia), Sent(ia + n));
    res = ranges::generate(rng, res.second);
    CHECK(ia[0] == 5);
    CHECK(ia[1] == 6);
    CHECK(ia[2] == 7);
    CHECK(ia[3] == 8);
    CHECK(res.first == Iter(ia + n));
    CHECK(res.second.i_ == 9);

    auto res2 = ranges::generate(std::move(rng), res.second);
    CHECK(ia[0] == 9);
    CHECK(ia[1] == 10);
    CHECK(ia[2] == 11);
    CHECK(ia[3] == 12);
    CHECK(res2.first.get_unsafe() == Iter(ia + n));
    CHECK(res2.second.i_ == 13);
}

void test2()
{
    // Test ranges::generate with a genuine output range
    std::vector<int> v;
    auto rng = ranges::view::counted(ranges::back_inserter(v), 5);
    ranges::generate(rng, gen_test(1));
    CHECK(v.size() == 5u);
    CHECK(v[0] == 1);
    CHECK(v[1] == 2);
    CHECK(v[2] == 3);
    CHECK(v[3] == 4);
    CHECK(v[4] == 5);
}

int main()
{
    test<forward_iterator<int*> >();
    test<bidirectional_iterator<int*> >();
    test<random_access_iterator<int*> >();
    test<int*>();

    test<forward_iterator<int*>, sentinel<int*> >();
    test<bidirectional_iterator<int*>, sentinel<int*> >();
    test<random_access_iterator<int*>, sentinel<int*> >();

    test2();

    return ::test_result();
}
