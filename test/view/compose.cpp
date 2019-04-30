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

#include <vector>

#include <range/v3/view/compose.hpp>
#include <range/v3/view/indirect.hpp>
#include <range/v3/view/const.hpp>
#include <range/v3/view/drop.hpp>
#include <range/v3/view/slice.hpp>
#include <range/v3/view/transform.hpp>

#include "../simple_test.hpp"
#include "../test_utils.hpp"

#if RANGES_CXX_VER >= RANGES_CXX_STD_17

using namespace ranges;


using List = std::vector<std::unique_ptr<int>>;
static List list;


struct nullary_test
{
    using Get = compose_view<List, view::indirect>;

    Get get_a(){ return list | view::indirect; }
    Get get_b(){ return Get(list); }

    using Get2 = compose_view<List, view::indirect, view::const_>;

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

    using Get = compose_view<List, view::indirect, compose_bind<view::transform, Fn>>;

    Get get_a()
    {
        return list | view::indirect | view::transform(Fn{});
    }
    Get get_b()
    {
        // Is it safe to do so?
        // Or remove this feature?
        return Get{list};
    }
    Get get_c()
    {
        return Get{list, view::transform(Fn{})};
    }
    Get get_d()
    {
        return Get{list, Fn()};
    }


    using Get2 = compose_view<List, compose_bind<view::drop, int>>;
    Get2 get2_a()
    {
        return list | view::drop(1);
    }
    Get2 get2_b()
    {
        return Get2{list, view::drop(1)};
    }
    Get2 get2_c()
    {
        return Get2{list, 1};
    }


    using Get3 = compose_view<
        List,
        view::indirect,
        compose_bind<view::slice, int, int>,
        compose_bind<view::transform, Fn>>;
    Get3 get3_a()
    {
        return Get3{list, 1, 3, Fn{}};
    }
    Get3 get3_b()
    {
        return Get3{list, view::slice(1, 3), view::transform(Fn{})};
    }
    Get3 get3_c()
    {
        // is this dangerous or not?
        return Get3{list};
    }


    using Get4 = compose_view<
        List,
        view::indirect,
        compose_bind<view::transform, Fn>,
        compose_bind<view::drop, int>>;
    Get4 get4_a()
    {
        return Get4{list, Fn{}, 1};
    }
    Get4 get4_b()
    {
        return Get4{list, view::transform(Fn{}), view::drop(1)};
    }


    // interesting example
    inline static auto fn = view::transform(Fn{}) | view::drop(1);
    using Get5 = compose_view<List, view::indirect, fn>;
    Get5 get5_a()
    {
        return Get5{list};  // (can't be done with types)
    }


    void test()
    {
        check_equal(get_a(), list | view::indirect);
        check_equal(get_a(), get_b());
        check_equal(get_b(), get_c());
        check_equal(get_c(), get_d());

        check_equal(get2_a(), get2_b());
        check_equal(get2_b(), get2_c());

        check_equal(get3_a(), get3_b());

        check_equal(get4_a(), get4_b());

        check_equal(get4_a(), get5_a());
    }
};


int main()
{
    list.emplace_back(new int(1));
    list.emplace_back(new int(2));
    list.emplace_back(new int(3));
    list.emplace_back(new int(4));

    nullary_test().test();
    binding_test().test();

    return test_result();
}

#else
int main() { return 0;}
#endif //RANGES_CXX_VER >= RANGES_CXX_STD_17