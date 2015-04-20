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

#include <utility>
#include <range/v3/core.hpp>
#include <range/v3/algorithm/find.hpp>
#include "../simple_test.hpp"
#include "../test_iterators.hpp"

struct S
{
    int i_;
};

#ifdef RANGES_CXX_GREATER_THAN_11
template<class Rng, class T>
RANGES_RELAXED_CONSTEXPR T ret_val(Rng r, T val) {
    auto rng = r;
    auto pi = ranges::find(rng, val);
    return *pi;
}
template<class Rng, class T>
RANGES_RELAXED_CONSTEXPR bool found(Rng r, T val) {
    auto rng = r;
    auto pi = ranges::find(rng, val);
    return pi != ranges::end(rng);
}
#endif

int main()
{
    using namespace ranges;

    int ia[] = {0, 1, 2, 3, 4, 5};
    constexpr unsigned s = size(ia);
    input_iterator<const int*> r = find(input_iterator<const int*>(ia),
                                        input_iterator<const int*>(ia+s), 3);
    CHECK(*r == 3);
    r = find(input_iterator<const int*>(ia),
             input_iterator<const int*>(ia+s), 10);
    CHECK(r == input_iterator<const int*>(ia+s));

    r = find(input_iterator<const int*>(ia),
             sentinel<const int*>(ia+s), 3);
    CHECK(*r == 3);
    r = find(input_iterator<const int*>(ia),
             sentinel<const int*>(ia+s), 10);
    CHECK(r == input_iterator<const int*>(ia+s));

    int *pi = find(ia, 3);
    CHECK(*pi == 3);
    pi = find(ia, 10);
    CHECK(pi == ia+s);

    auto pj = find(view::all(ia), 3);
    CHECK(*pj.get_unsafe() == 3);
    pj = find(view::all(ia), 10);
    CHECK(pj.get_unsafe() == ia+s);

    S sa[] = {{0}, {1}, {2}, {3}, {4}, {5}};
    S *ps = find(sa, 3, &S::i_);
    CHECK(ps->i_ == 3);
    ps = find(sa, 10, &S::i_);
    CHECK(ps == end(sa));

#ifdef RANGES_CXX_GREATER_THAN_11
    {
        static_assert(ret_val(std::initializer_list<int>{1, 2}, 2) == 2, "");
        static_assert(found(std::initializer_list<int>{1, 3, 4}, 4), "");
        static_assert(!found(std::initializer_list<int>{1, 3, 4}, 5), "");
    }
#endif

    return ::test_result();
}
