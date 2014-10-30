// Range v3 library
//
//  Copyright Eric Niebler 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <vector>
#include <range/v3/core.hpp>
#include <range/v3/view/iota.hpp>
#include <range/v3/view/repeat_n.hpp>
#include <range/v3/view/for_each.hpp>
#include <range/v3/algorithm/random_shuffle.hpp>
#include <range/v3/algorithm/equal.hpp>
#include <range/v3/algorithm/is_sorted.hpp>
#include <range/v3/container/sort.hpp>
#include <range/v3/container/unique.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"

int main()
{
    using namespace ranges;

    // [1,2,2,3,3,3,4,4,4,4,5,5,5,5,5,...]
    std::vector<int> v =
        view::for_each(view::ints(1,99), [](int i){
            return view::repeat_n(i,i);
        });
    random_shuffle(v);
    CHECK(!is_sorted(v));

    v |= cont::sort | cont::unique;
    CHECK(equal(v, view::ints(1,99)));

    return ::test_result();
}
