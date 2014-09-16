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
#include <range/v3/algorithm/generate_n.hpp>
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

template <class Iter, class Sent = Iter>
void
test()
{
    const unsigned n = 4;
    int ia[n] = {0};
    std::pair<Iter, gen_test> res = ranges::generate_n(Iter(ia), n, gen_test(1));
    CHECK(ia[0] == 1);
    CHECK(ia[1] == 2);
    CHECK(ia[2] == 3);
    CHECK(ia[3] == 4);
    CHECK(res.first == Iter(ia + n));
    CHECK(res.second.i_ == 5);

    auto rng = ranges::make_range(Iter(ia), Sent(ia + n - 1));
    std::tuple<Iter, gen_test, std::ptrdiff_t> res2 = ranges::generate_n(rng, n, res.second);
    CHECK(ia[0] == 5);
    CHECK(ia[1] == 6);
    CHECK(ia[2] == 7);
    CHECK(ia[3] == 4);
    CHECK(std::get<0>(res2) == Iter(ia + n - 1));
    CHECK(std::get<1>(res2).i_ == 8);
    CHECK(std::get<2>(res2) == 1);

    rng = ranges::make_range(Iter(ia), Sent(ia + n));
    res2 = ranges::generate_n(rng, n - 1, std::get<1>(res2));
    CHECK(ia[0] == 8);
    CHECK(ia[1] == 9);
    CHECK(ia[2] == 10);
    CHECK(ia[3] == 4);
    CHECK(std::get<0>(res2) == Iter(ia + n - 1));
    CHECK(std::get<1>(res2).i_ == 11);
    CHECK(std::get<2>(res2) == 0);
}

void test2()
{
    // Test ranges::generate with a genuine output range
    std::vector<int> v;
    ranges::generate_n(ranges::back_inserter(v), 5, gen_test(1));
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
