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
#include <range/v3/algorithm/find_first_of.hpp>
#include "../simple_test.hpp"
#include "../test_iterators.hpp"
#include "../test_utils.hpp"

namespace rng = ranges;

void test_iter()
{
    using namespace ranges;
    int ia[] = {0, 1, 2, 3, 0, 1, 2, 3};
    static constexpr unsigned sa = size(ia);
    int ib[] = {1, 3, 5, 7};
    static constexpr unsigned sb = size(ib);
    CHECK(rng::find_first_of(input_iterator<const int*>(ia),
                             sentinel<const int*>(ia + sa),
                             forward_iterator<const int*>(ib),
                             sentinel<const int*>(ib + sb)) ==
                             input_iterator<const int*>(ia+1));
    int ic[] = {7};
    CHECK(rng::find_first_of(input_iterator<const int*>(ia),
                             sentinel<const int*>(ia + sa),
                             forward_iterator<const int*>(ic),
                             sentinel<const int*>(ic + 1)) ==
                             input_iterator<const int*>(ia+sa));
    CHECK(rng::find_first_of(input_iterator<const int*>(ia),
                             sentinel<const int*>(ia + sa),
                             forward_iterator<const int*>(ic),
                             sentinel<const int*>(ic)) ==
                             input_iterator<const int*>(ia+sa));
    CHECK(rng::find_first_of(input_iterator<const int*>(ia),
                             sentinel<const int*>(ia),
                             forward_iterator<const int*>(ic),
                             sentinel<const int*>(ic+1)) ==
                             input_iterator<const int*>(ia));
}

void test_iter_pred()
{
    using namespace ranges;
    int ia[] = {0, 1, 2, 3, 0, 1, 2, 3};
    static constexpr unsigned sa = size(ia);
    int ib[] = {1, 3, 5, 7};
    static constexpr unsigned sb = size(ib);
    CHECK(rng::find_first_of(input_iterator<const int*>(ia),
                             sentinel<const int*>(ia + sa),
                             forward_iterator<const int*>(ib),
                             sentinel<const int*>(ib + sb),
                             std::equal_to<int>()) ==
                             input_iterator<const int*>(ia+1));
    int ic[] = {7};
    CHECK(rng::find_first_of(input_iterator<const int*>(ia),
                             sentinel<const int*>(ia + sa),
                             forward_iterator<const int*>(ic),
                             sentinel<const int*>(ic + 1),
                             std::equal_to<int>()) ==
                             input_iterator<const int*>(ia+sa));
    CHECK(rng::find_first_of(input_iterator<const int*>(ia),
                             sentinel<const int*>(ia + sa),
                             forward_iterator<const int*>(ic),
                             sentinel<const int*>(ic),
                             std::equal_to<int>()) ==
                             input_iterator<const int*>(ia+sa));
    CHECK(rng::find_first_of(input_iterator<const int*>(ia),
                             sentinel<const int*>(ia),
                             forward_iterator<const int*>(ic),
                             sentinel<const int*>(ic+1),
                             std::equal_to<int>()) ==
                             input_iterator<const int*>(ia));
}

void test_rng()
{
    using namespace ranges;
    int ia[] = {0, 1, 2, 3, 0, 1, 2, 3};
    static constexpr unsigned sa = size(ia);
    int ib[] = {1, 3, 5, 7};
    static constexpr unsigned sb = size(ib);
    CHECK(rng::find_first_of(as_lvalue(make_iterator_range(input_iterator<const int*>(ia),
                             input_iterator<const int*>(ia + sa))),
                             make_iterator_range(forward_iterator<const int*>(ib),
                             forward_iterator<const int*>(ib + sb))) ==
                             input_iterator<const int*>(ia+1));
    CHECK(rng::find_first_of(make_iterator_range(input_iterator<const int*>(ia),
                             input_iterator<const int*>(ia + sa)),
                             make_iterator_range(forward_iterator<const int*>(ib),
                             forward_iterator<const int*>(ib + sb))).get_unsafe() ==
                             input_iterator<const int*>(ia+1));
    int ic[] = {7};
    CHECK(rng::find_first_of(as_lvalue(make_iterator_range(input_iterator<const int*>(ia),
                             input_iterator<const int*>(ia + sa))),
                             make_iterator_range(forward_iterator<const int*>(ic),
                             forward_iterator<const int*>(ic + 1))) ==
                             input_iterator<const int*>(ia+sa));
    CHECK(rng::find_first_of(as_lvalue(make_iterator_range(input_iterator<const int*>(ia),
                             input_iterator<const int*>(ia + sa))),
                             make_iterator_range(forward_iterator<const int*>(ic),
                             forward_iterator<const int*>(ic))) ==
                             input_iterator<const int*>(ia+sa));
    CHECK(rng::find_first_of(as_lvalue(make_iterator_range(input_iterator<const int*>(ia),
                             input_iterator<const int*>(ia))),
                             make_iterator_range(forward_iterator<const int*>(ic),
                             forward_iterator<const int*>(ic+1))) ==
                             input_iterator<const int*>(ia));
    CHECK(rng::find_first_of(make_iterator_range(input_iterator<const int*>(ia),
                             input_iterator<const int*>(ia + sa)),
                             make_iterator_range(forward_iterator<const int*>(ic),
                             forward_iterator<const int*>(ic + 1))).get_unsafe() ==
                             input_iterator<const int*>(ia+sa));
    CHECK(rng::find_first_of(make_iterator_range(input_iterator<const int*>(ia),
                             input_iterator<const int*>(ia + sa)),
                             make_iterator_range(forward_iterator<const int*>(ic),
                             forward_iterator<const int*>(ic))).get_unsafe() ==
                             input_iterator<const int*>(ia+sa));
    CHECK(rng::find_first_of(make_iterator_range(input_iterator<const int*>(ia),
                             input_iterator<const int*>(ia)),
                             make_iterator_range(forward_iterator<const int*>(ic),
                             forward_iterator<const int*>(ic+1))).get_unsafe() ==
                             input_iterator<const int*>(ia));
}

void test_rng_pred()
{
    using namespace ranges;
    int ia[] = {0, 1, 2, 3, 0, 1, 2, 3};
    static constexpr unsigned sa = size(ia);
    int ib[] = {1, 3, 5, 7};
    static constexpr unsigned sb = size(ib);
    CHECK(rng::find_first_of(as_lvalue(make_iterator_range(input_iterator<const int*>(ia),
                             input_iterator<const int*>(ia + sa))),
                             make_iterator_range(forward_iterator<const int*>(ib),
                             forward_iterator<const int*>(ib + sb)),
                             std::equal_to<int>()) ==
                             input_iterator<const int*>(ia+1));
    int ic[] = {7};
    CHECK(rng::find_first_of(as_lvalue(make_iterator_range(input_iterator<const int*>(ia),
                             input_iterator<const int*>(ia + sa))),
                             make_iterator_range(forward_iterator<const int*>(ic),
                             forward_iterator<const int*>(ic + 1)),
                             std::equal_to<int>()) ==
                             input_iterator<const int*>(ia+sa));
    CHECK(rng::find_first_of(as_lvalue(make_iterator_range(input_iterator<const int*>(ia),
                             input_iterator<const int*>(ia + sa))),
                             make_iterator_range(forward_iterator<const int*>(ic),
                             forward_iterator<const int*>(ic)),
                             std::equal_to<int>()) ==
                             input_iterator<const int*>(ia+sa));
    CHECK(rng::find_first_of(as_lvalue(make_iterator_range(input_iterator<const int*>(ia),
                             input_iterator<const int*>(ia))),
                             make_iterator_range(forward_iterator<const int*>(ic),
                             forward_iterator<const int*>(ic+1)),
                             std::equal_to<int>()) ==
                             input_iterator<const int*>(ia));
}

struct S
{
    int i;
};

void test_rng_pred_proj()
{
    using namespace ranges;
    S ia[] = {S{0}, S{1}, S{2}, S{3}, S{0}, S{1}, S{2}, S{3}};
    static constexpr unsigned sa = size(ia);
    S ib[] = {S{1}, S{3}, S{5}, S{7}};
    static constexpr unsigned sb = size(ib);
    CHECK(rng::find_first_of(as_lvalue(make_iterator_range(input_iterator<const S*>(ia),
                             input_iterator<const S*>(ia + sa))),
                             make_iterator_range(forward_iterator<const S*>(ib),
                             forward_iterator<const S*>(ib + sb)),
                             std::equal_to<int>(), &S::i, &S::i) ==
                             input_iterator<const S*>(ia+1));
    S ic[] = {S{7}};
    CHECK(rng::find_first_of(as_lvalue(make_iterator_range(input_iterator<const S*>(ia),
                             input_iterator<const S*>(ia + sa))),
                             make_iterator_range(forward_iterator<const S*>(ic),
                             forward_iterator<const S*>(ic + 1)),
                             std::equal_to<int>(), &S::i, &S::i) ==
                             input_iterator<const S*>(ia+sa));
    CHECK(rng::find_first_of(as_lvalue(make_iterator_range(input_iterator<const S*>(ia),
                             input_iterator<const S*>(ia + sa))),
                             make_iterator_range(forward_iterator<const S*>(ic),
                             forward_iterator<const S*>(ic)),
                             std::equal_to<int>(), &S::i, &S::i) ==
                             input_iterator<const S*>(ia+sa));
    CHECK(rng::find_first_of(as_lvalue(make_iterator_range(input_iterator<const S*>(ia),
                             input_iterator<const S*>(ia))),
                             make_iterator_range(forward_iterator<const S*>(ic),
                             forward_iterator<const S*>(ic+1)),
                             std::equal_to<int>(), &S::i, &S::i) ==
                             input_iterator<const S*>(ia));
}


int main()
{
    ::test_iter();
    ::test_iter_pred();
    ::test_rng();
    ::test_rng_pred();
    ::test_rng_pred_proj();
    return ::test_result();
}
