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

#include <range/v3/view/compose.hpp>

#include <vector>
#include <memory>

#include <range/v3/view/indirect.hpp>
#include <range/v3/view/const.hpp>
#include <range/v3/view/tail.hpp>
#include <range/v3/view/transform.hpp>

#include "../simple_test.hpp"
#include "../test_utils.hpp"

using namespace ranges;

using List = std::vector<std::unique_ptr<int>>;
List list;

struct nullary_test
{
    using Get = compose_view<List, indirect_view>;

    Get get_a(){ return list | view::indirect; }
    Get get_b(){ return Get{list}; }

    using Get2 = compose_view<List, indirect_view, const_view>;

    Get2 get2_a() { return list | view::indirect | view::const_; }
    Get2 get2_b() { return Get2{list}; }

    void test()
    {
        check_equal(get_a(), get_b());
        check_equal(get2_a(), get2_b());
    }
};

struct binding_test
{
    struct Fn
    {
        const int& operator()(const int& i) const { return i; }
    };

    using Get = compose_view_t< List, indirect_view, compose_bind<transform_view, Fn>::type >;

    Get get()
    {
        return list | view::indirect | view::transform(Fn{});

        // Could be:
        // return Get{list};
        //
        // if transform_view (and others) would have explicit constructor for trivial Fun's :
        // explicit transform_view(Rng rng) : transform_view(rng, Fun{}) {}
    }

    using sample = transform_view<indirect_view<view::all_t<List>>, Fn>;
    static_assert( std::is_same<Get, sample>::value, "" );

    void test()
    {
        get();
    }
};

int main()
{
    nullary_test().test();
    binding_test().get();

    return test_result();
}
