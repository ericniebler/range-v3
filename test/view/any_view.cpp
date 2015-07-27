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

#include <range/v3/core.hpp>
#include <range/v3/view/iota.hpp>
#include <range/v3/view/take.hpp>
#include <range/v3/view/reverse.hpp>
#include <range/v3/view/any_view.hpp>
#include <range/v3/utility/copy.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"

int main()
{
    using namespace ranges;

    any_view<int> ints = view::ints;
    ::models<concepts::InputView>(ints);
    ::models_not<concepts::ForwardView>(ints);
    ::check_equal(ints | view::take(10), {0,1,2,3,4,5,6,7,8,9});
    ::check_equal(ints | view::take(10), {0,1,2,3,4,5,6,7,8,9});

    any_view<int> ints2 = view::ints | view::take(10);
    ::models<concepts::InputView>(ints2);
    ::models_not<concepts::ForwardView>(ints2);
    ::check_equal(ints2, {0,1,2,3,4,5,6,7,8,9});
    ::check_equal(ints2, {0,1,2,3,4,5,6,7,8,9});

    any_random_access_view<int> ints3 = view::ints | view::take(10);
    ::models<concepts::RandomAccessView>(ints3);
    ::check_equal(ints3, {0,1,2,3,4,5,6,7,8,9});
    ::check_equal(ints3, {0,1,2,3,4,5,6,7,8,9});
    ::check_equal(aux::copy(ints3), {0,1,2,3,4,5,6,7,8,9});
    ::check_equal(ints3 | view::reverse, {9,8,7,6,5,4,3,2,1,0});

    any_view<int&> e;
    CHECK(e.begin() == e.begin());
    CHECK(e.begin() == e.end());

    range_iterator_t<any_view<int&>> i{},j{};
    range_sentinel_t<any_view<int&>> k{};
    CHECK(i == j);
    CHECK(i == k);

    return test_result();
}
