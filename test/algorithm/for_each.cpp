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

#include <vector>
#include <range/v3/core.hpp>
#include <range/v3/algorithm/for_each.hpp>
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
    auto fun = [&](int i){sum += i; };
    std::vector<int> v1 { 0, 2, 4, 6 };
    CHECK(ranges::for_each(v1.begin(), v1.end(), fun).in() == v1.end());
    CHECK(ranges::for_each(v1, fun).in() == v1.end());
    CHECK(sum == 24);

    sum = 0;
    auto rfun = [&](int & i){sum += i; };
    CHECK(ranges::for_each(v1.begin(), v1.end(), rfun).in() == v1.end());
    CHECK(ranges::for_each(v1, rfun).in() == v1.end());
    CHECK(sum == 24);

    sum = 0;
    std::vector<S> v2{{&sum, 0}, {&sum, 2}, {&sum, 4}, {&sum, 6}};
    CHECK(ranges::for_each(v2.begin(), v2.end(), &S::p).in() == v2.end());
    CHECK(ranges::for_each(v2, &S::p).in() == v2.end());
    CHECK(sum == 24);

    sum = 0;
    CHECK(ranges::for_each(ranges::make_iterator_range(v1.begin(), v1.end()), fun).in().get_unsafe() == v1.end());
    CHECK(sum == 12);

    return ::test_result();
}
