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
        return Get{list};
    }

    void test()
    {
        check_equal(get_a(), list | view::indirect);
        check_equal(get_a(), get_b());
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