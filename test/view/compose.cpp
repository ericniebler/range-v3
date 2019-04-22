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

#include "../simple_test.hpp"
#include "../test_utils.hpp"

using namespace ranges;

void type_test()
{
    using List = std::vector<std::unique_ptr<int>>;
    {
        using Get = compose_view<List, indirect_view>::type;
        using sample = indirect_view<view::all_t<List>>;
        CHECK(std::is_same<Get, sample>::value);
    }
    {
        using Get = compose_view<List, indirect_view, const_view >::type;
        using sample = const_view<indirect_view<view::all_t<List>>>;
        CHECK(std::is_same<Get, sample>::value);
    }

}

struct nullary_test
{
    using List = std::vector<std::unique_ptr<int>>;
    List list;

    using Get = compose_view<List, indirect_view/*, const_view */>;
    //Get get();

    indirect_view<view::all_t<List>> get(){
        return list | view::indirect /*| view::const_*/;
    }
};

int main()
{
    type_test();
    nullary_test().get();

    return test_result();
}

