// Range v3 library
//
//  Copyright Eric Niebler 2014-present
//  Copyright Rostislav Khlebnikov 2017
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3

#include <vector>
#include <range/v3/core.hpp>
#include <range/v3/algorithm/for_each_n.hpp>
#include "../simple_test.hpp"

struct S
{
    void p() const { *p_ += i_; }
    int *p_;
    int i_;
};

int main()
{
    int sum = 0;
    auto const fun = [&](int i){ sum += i; };
    std::vector<int> v1 { 1, 2, 4, 6 };
    CHECK(ranges::for_each_n(v1.begin(), 2, fun) == v1.begin() + 2);
    CHECK(ranges::for_each_n(v1, 2, fun) == v1.begin() + 2);
    CHECK(sum == 3 * 2);

    sum = 0;
    auto const rfun = [&](int & i){ sum += i; };
    auto const sz = static_cast<int>(v1.size());
    CHECK(ranges::for_each_n(v1.begin(), sz, rfun) == v1.end());
    CHECK(ranges::for_each_n(v1, sz, rfun) == v1.end());
    CHECK(sum == 13 * 2);

    sum = 0;
    std::vector<S> v2{{&sum, 1}, {&sum, 2}, {&sum, 4}, {&sum, 6}};
    CHECK(ranges::for_each_n(v2.begin(), 3, &S::p) == v2.begin() + 3);
    CHECK(ranges::for_each_n(v2, 3, &S::p) == v2.begin() + 3);
    CHECK(sum == 7 * 2);

    sum = 0;
    CHECK(ranges::for_each_n(v2.begin(), 4, fun, &S::i_) == v2.begin() + 4);
    CHECK(ranges::for_each_n(v2, 4, fun, &S::i_) == v2.begin() + 4);
    CHECK(sum == 13 * 2);


    return ::test_result();
}
