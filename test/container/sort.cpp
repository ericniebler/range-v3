// Range v3 library
//
//  Copyright Eric Niebler 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <array>
#include <vector>
#include <range/v3/core.hpp>
#include <range/v3/view/iota.hpp>
#include <range/v3/algorithm/random_shuffle.hpp>
#include <range/v3/algorithm/copy.hpp>
#include <range/v3/algorithm/move.hpp>
#include <range/v3/algorithm/is_sorted.hpp>
#include <range/v3/algorithm/equal.hpp>
#include <range/v3/container/sort.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"

int main()
{
    using namespace ranges;

    std::vector<int> v = view::ints(0,99);
    random_shuffle(v);
    CHECK(!is_sorted(v));

    auto v2 = v | copy | cont::sort;
    CHECK(size(v2) == size(v));
    CHECK(is_sorted(v2));
    CHECK(!is_sorted(v));
    ::models<concepts::Same>(v, v2);

    v |= cont::sort;
    CHECK(is_sorted(v));

    random_shuffle(v);
    CHECK(!is_sorted(v));

    v = v | move | cont::sort;
    CHECK(is_sorted(v));
    CHECK(equal(v, v2));

    return ::test_result();
}
