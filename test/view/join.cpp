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

#include <vector>
#include <iterator>
#include <functional>
#include <range/v3/core.hpp>
#include <range/v3/view/join.hpp>
#include <range/v3/view/split.hpp>
#include <range/v3/view/generate_n.hpp>
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

int main()
{
    using namespace ranges;

    // Test that we can join an input range of input ranges:
    auto rng0 = make_input_rng() | view::join;
    models<concepts::InputIterable>(rng0);
    models_not<concepts::ForwardIterable>(rng0);
    models_not<concepts::BoundedIterable>(rng0);
    models_not<concepts::SizedIterable>(rng0);
    check_equal(rng0, {0,1,2,3,4,5,6,7,8});

    // Joining with a value
    N = 0;
    auto rng1 = make_input_rng() | view::join(42);
    models<concepts::InputIterable>(rng1);
    models_not<concepts::ForwardIterable>(rng1);
    models_not<concepts::BoundedIterable>(rng1);
    models_not<concepts::SizedIterable>(rng1);
    check_equal(rng1, {0,1,2,42,3,4,5,42,6,7,8});

    // Joining with a range
    N = 0;
    int rgi[] = {42,43};
    auto rng2 = make_input_rng() | view::join(rgi);
    models<concepts::InputIterable>(rng2);
    models_not<concepts::ForwardIterable>(rng2);
    models_not<concepts::BoundedIterable>(rng2);
    models_not<concepts::SizedIterable>(rng2);
    check_equal(rng2, {0,1,2,42,43,3,4,5,42,43,6,7,8});

    // Just for fun:
    std::string str = "Now,is,the,time,for,all,good,men,to,come,to,the,aid,of,their,country";
    std::string res = str | view::split(',') | view::join(' ');
    CHECK(res == "Now is the time for all good men to come to the aid of their country");

    std::vector<std::string> vs{"This","is","his","face"};
    auto rng3 = view::join(vs);
    models<concepts::SizedIterable>(rng3);
    CHECK(rng3.size() == 13u);
    CHECK(to_<std::string>(rng3) == "Thisishisface");

    auto rng4 = view::join(vs, ' ');
    models<concepts::SizedIterable>(rng4);
    CHECK(rng4.size() == 16u);
    CHECK(to_<std::string>(rng4) == "This is his face");

    return ::test_result();
}
