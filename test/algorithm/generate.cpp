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
#include <range/v3/algorithm/generate.hpp>
#include <range/v3/view/counted.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"
#include "../test_iterators.hpp"

struct gen_test
{
    int i_;
    RANGES_RELAXED_CONSTEXPR gen_test() : i_{} {}
    RANGES_RELAXED_CONSTEXPR gen_test(int i) : i_(i) {}
    RANGES_RELAXED_CONSTEXPR int operator()() {return i_++;}
};

template <class Iter, class Sent = Iter>
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

    auto rng = ranges::make_range(Iter(ia), Sent(ia + n));
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

#ifdef RANGES_CXX_GREATER_THAN_11

template <class Iter, class Sent = Iter>
RANGES_RELAXED_CONSTEXPR bool constexpr_test() {
    bool r = true;
    const unsigned n = 4;
    int ia[n] = {0};
    std::pair<Iter, gen_test> res(ranges::generate(Iter(ia), Sent(ia + n), gen_test(1)));
    if(ia[0] != 1) { r = false; };
    if(ia[1] != 2){ r = false; };
    if(ia[2] != 3){ r = false; };
    if(ia[3] != 4){ r = false; };
    if(res.first != Iter(ia + n)){ r = false; };
    if(res.second.i_ != 5){ r = false; };

    auto rng = ranges::make_range(Iter(ia), Sent(ia + n));
    auto res2(ranges::generate(rng, res.second));
    if(ia[0] != 5){ r = false; };
    if(ia[1] != 6){ r = false; };
    if(ia[2] != 7){ r = false; };
    if(ia[3] != 8){ r = false; };
    if(res2.first != Iter(ia + n)){ r = false; };
    if(res2.second.i_ != 9){ r = false; };

    auto res3(ranges::generate(std::move(rng), res2.second));
    if(ia[0] != 9){ r = false; };
    if(ia[1] != 10){ r = false; };
    if(ia[2] != 11){ r = false; };
    if(ia[3] != 12){ r = false; };
    if(res3.first.get_unsafe() != Iter(ia + n)){ r = false; };
    if(res3.second.i_ != 13){ r = false; };
    return r;
}

#endif

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

#ifdef RANGES_CXX_GREATER_THAN_11
    static_assert(constexpr_test<forward_iterator<int*> >(), "");
    static_assert(constexpr_test<bidirectional_iterator<int*> >(), "");
    static_assert(constexpr_test<random_access_iterator<int*> >(), "");
    static_assert(constexpr_test<int*>(), "");
    static_assert(constexpr_test<forward_iterator<int*>, sentinel<int*> >(), "");
    static_assert(constexpr_test<bidirectional_iterator<int*>, sentinel<int*> >(), "");
    static_assert(constexpr_test<random_access_iterator<int*>, sentinel<int*> >(), "");
#endif
    return ::test_result();
}
