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

// Work around strange glibc bug(?)
#include <iosfwd>
#if defined(RANGES_CXX_GREATER_THAN_11) && defined(__GLIBCXX__)
int gets;
#endif

#include <vector>
#include <iterator>
#include <functional>
#include <range/v3/core.hpp>
#include <range/v3/view/join.hpp>
#include <range/v3/view/split.hpp>
#include <range/v3/view/generate_n.hpp>
#include <range/v3/view/repeat_n.hpp>
#include <range/v3/view/concat.hpp>
#include <range/v3/view/single.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"
#include "../test_iterators.hpp"

static int N = 0;
auto const make_input_rng = []
{
    using ranges::view::generate_n;
    return generate_n([](){
        return generate_n([](){
            return N++;
        },3);
    },3);
};

template<typename T>
auto twice(T t) -> decltype(ranges::view::concat(ranges::view::single(t), ranges::view::single(t)))
{
    return ranges::view::concat(ranges::view::single(t), ranges::view::single(t));
}

int main()
{
    using namespace ranges;

    // Test that we can join an input range of input ranges:
    auto rng0 = make_input_rng() | view::join;
    static_assert(range_cardinality<decltype(rng0)>::value == ranges::finite, "");
    models<concepts::InputRange>(rng0);
    models_not<concepts::ForwardRange>(rng0);
    models_not<concepts::BoundedRange>(rng0);
    models_not<concepts::SizedRange>(rng0);
    check_equal(rng0, {0,1,2,3,4,5,6,7,8});

    // Joining with a value
    N = 0;
    auto rng1 = make_input_rng() | view::join(42);
    static_assert(range_cardinality<decltype(rng1)>::value == ranges::finite, "");
    models<concepts::InputRange>(rng1);
    models_not<concepts::ForwardRange>(rng1);
    models_not<concepts::BoundedRange>(rng1);
    models_not<concepts::SizedRange>(rng1);
    check_equal(rng1, {0,1,2,42,3,4,5,42,6,7,8});

    // Joining with a range
    N = 0;
    int rgi[] = {42,43};
    auto rng2 = make_input_rng() | view::join(rgi);
    static_assert(range_cardinality<decltype(rng2)>::value == ranges::finite, "");
    models<concepts::InputRange>(rng2);
    models_not<concepts::ForwardRange>(rng2);
    models_not<concepts::BoundedRange>(rng2);
    models_not<concepts::SizedRange>(rng2);
    check_equal(rng2, {0,1,2,42,43,3,4,5,42,43,6,7,8});

    // Just for fun:
    std::string str = "Now,is,the,time,for,all,good,men,to,come,to,the,aid,of,their,country";
    std::string res = str | view::split(',') | view::join(' ');
    CHECK(res == "Now is the time for all good men to come to the aid of their country");
    static_assert(range_cardinality<decltype(res)>::value == ranges::finite, "");

    std::vector<std::string> vs{"This","is","his","face"};
    auto rng3 = view::join(vs);
    static_assert(range_cardinality<decltype(rng3)>::value == ranges::finite, "");
    models_not<concepts::SizedRange>(rng3);
    models_not<concepts::SizedIteratorRange>(begin(rng3), end(rng3));
    CHECK(to_<std::string>(rng3) == "Thisishisface");

    auto rng4 = view::join(vs, ' ');
    static_assert(range_cardinality<decltype(rng3)>::value == ranges::finite, "");
    models_not<concepts::SizedRange>(rng4);
    models_not<concepts::SizedIteratorRange>(begin(rng4), end(rng4));
    CHECK(to_<std::string>(rng4) == "This is his face");

    auto rng5 = view::join(twice(twice(42)));
    static_assert(range_cardinality<decltype(rng5)>::value == 4, "");
    models<concepts::SizedRange>(rng5);
    CHECK(rng5.size() == 4u);
    static_assert(rng5.size() == 4u, "");
    check_equal(rng5, {42,42,42,42});

    auto rng6 = view::join(twice(view::repeat_n(42, 2)));
    static_assert(range_cardinality<decltype(rng6)>::value == ranges::finite, "");
    models<concepts::SizedRange>(rng6);
    CHECK(rng6.size() == 4u);
    check_equal(rng6, {42,42,42,42});

    return ::test_result();
}
