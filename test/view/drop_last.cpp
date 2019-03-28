/// \file
// Range v3 library
//
//  Copyright Andrey Diduh 2019
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#include "../simple_test.hpp"
#include "../test_utils.hpp"

#include <vector>
#include <list>
#include <forward_list>

#include <range/v3/view/drop_last.hpp>

using namespace ranges;

template<class Rng>
void test_range(Rng&& src){
    {
        auto list = src | view::drop_last(2);
        ::check_equal(list, {1,2});
    }
    {
        auto list = src | view::drop_last(0);
        ::check_equal(list, {1,2,3,4});
    }
    {
        auto list = src | view::drop_last(4);
        CHECK(list.empty());
    }
}

void random_acccess_test(){
    using Src = std::vector<int>;
    static_assert(
        ranges::RandomAccessRange<Src>().value
        , "Must be exactly RA.");
    static_assert(
        std::is_same<
            drop_last_view<Src>, drop_last_bidirectional_view<Src>
        >::value
        , "Must have correct view.");

    Src src = {1,2,3,4};

    test_range(src);
}

void bidirectional_test(){
    using Src = std::list<int>;
    static_assert(
        !ranges::RandomAccessRange<Src>().value &&
        ranges::BidirectionalRange<Src>().value
        , "Must be exactly bidirectional.");
    static_assert(
        std::is_same<
            drop_last_view<Src>, drop_last_bidirectional_view<Src>
        >::value
        , "Must have correct view.");

    Src src = {1,2,3,4};

    test_range(src);
}

void forward_test(){
    using Src = std::forward_list<int>;
    static_assert(
        !ranges::BidirectionalRange<Src>().value &&
        ranges::ForwardRange<Src>().value
        , "Must be exactly forward.");
    static_assert(
        std::is_same<
            drop_last_view<Src>, drop_last_forward_view<Src>
        >::value
        , "Must have correct view.");

    Src src = {1,2,3,4};

    test_range(src);
}

int main()
{
    random_acccess_test();
    /*bidirectional_test();
    forward_test();*/

    return test_result();
}