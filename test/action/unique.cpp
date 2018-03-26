// Range v3 library
//
//  Copyright Eric Niebler 2014-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <random>
#include <vector>
#include <range/v3/core.hpp>
#include <range/v3/view/iota.hpp>
#include <range/v3/view/repeat_n.hpp>
#include <range/v3/view/for_each.hpp>
#include <range/v3/view/take.hpp>
#include <range/v3/algorithm/shuffle.hpp>
#include <range/v3/algorithm/equal.hpp>
#include <range/v3/algorithm/is_sorted.hpp>
#include <range/v3/action/shuffle.hpp>
#include <range/v3/action/sort.hpp>
#include <range/v3/action/unique.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"

int main()
{
    using namespace ranges;
    std::mt19937 gen;

    // [1,2,2,3,3,3,4,4,4,4,5,5,5,5,5,...]
    std::vector<int> v =
        view::for_each(view::ints(1,100), [](int i){
            return yield_from(view::repeat_n(i,i));
        });
    check_equal(view::take(v, 15), {1,2,2,3,3,3,4,4,4,4,5,5,5,5,5});
    v |= action::shuffle(gen);
    CHECK(!is_sorted(v));

    v |= action::sort | action::unique;
    CHECK(equal(v, view::ints(1,100)));

    return ::test_result();
}
