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

#include <utility>
#include <range/v3/core.hpp>
#include <range/v3/algorithm/find_if.hpp>
#include "../simple_test.hpp"
#include "../test_iterators.hpp"

struct S
{
    int i_;
};

int main()
{
    using namespace ranges;

    int ia[] = {0, 1, 2, 3, 4, 5};
    constexpr auto s = size(ia);
    input_iterator<const int*> r = find_if(input_iterator<const int*>(ia),
                                           input_iterator<const int*>(ia + s),
                                           [](int i){return i == 3;});
    CHECK(*r == 3);
    r = find_if(input_iterator<const int*>(ia),
                input_iterator<const int*>(ia+s),
                [](int i){return i == 10;});
    CHECK(r == input_iterator<const int*>(ia+s));

    r = find_if(input_iterator<const int*>(ia),
                sentinel<const int*>(ia+s),
                [](int i){return i == 3;});
    CHECK(*r == 3);
    r = find_if(input_iterator<const int*>(ia),
                sentinel<const int*>(ia+s),
                [](int i){return i == 10;});
    CHECK(r == input_iterator<const int*>(ia+s));

    int *pi = find_if(ia, [](int i){return i == 3;});
    CHECK(*pi == 3);
    pi = find_if(ia, [](int i){return i == 10;});
    CHECK(pi == ia+s);

    auto pj = find_if(view::all(ia), [](int i){return i == 3;});
    CHECK(*pj.get_unsafe() == 3);
    pj = find_if(view::all(ia), [](int i){return i == 10;});
    CHECK(pj.get_unsafe() == ia+s);

    S sa[] = {{0}, {1}, {2}, {3}, {4}, {5}};
    S *ps = find_if(sa, [](int i){return i == 3;}, &S::i_);
    CHECK(ps->i_ == 3);
    ps = find_if(sa, [](int i){return i == 10;}, &S::i_);
    CHECK(ps == end(sa));

    return ::test_result();
}
