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

//void type_test()
//{
//    using List = std::vector<std::unique_ptr<int>>;
//    /*{
//        using Get = compose_view<List, indirect_view>::type;
//        using sample = indirect_view<view::all_t<List>>;
//        CHECK(std::is_same<Get, sample>::value);
//    }*/
//    {
//        //using Get = compose_view<List, indirect_view, const_view, tail_view>::type;
//        using sample = tail_view<const_view<indirect_view<view::all_t<List>>>>;
//
//
//        using Get2 = compose_view<2, List, indirect_view, const_view, tail_view>::type;
//
//
//        //using T = typename get_n<0, indirect_view, const_view, tail_view>:: template type<List>;
//
//        std::cout << typeid(Get2).name();
//
//        CHECK(std::is_same<Get2, sample>::value);
//    }
//
//
//}

/*struct nullary_test
{
    using List = std::vector<std::unique_ptr<int>>;
    List list;

    using Get = compose_view<List, indirect_view>;
    //Get get();

    indirect_view<view::all_t<List>> get(){
        return list | view::indirect ;
    }
};
*/

struct binding_test
{
    using List = std::vector<std::unique_ptr<int>>;
    List list;

    struct Fn{
        const int& operator()(const int& i) const {
            return i;
        }
    };

    template<class R>
    using t = transform_view<R, Fn>;

    /*using Get = compose_v<
            List, indirect_view, compose_bind<transform_view, Fn>::type>;*/

    using Get = compose_view<List, indirect_view, const_view>;
    //Get get();

    //using R = transform_view< indirect_view<view::all_t<List>>, Fn>;

    Get get(){
        //return list | view::indirect | view::transform(Fn{});
        //Get g = Get{list};
        //return g;
        //return make_view<Get>(list);
        return list | view::indirect | view::const_;
        //return make<Get>(list);
    }
};



int main()
{
    //type_test();
    //nullary_test().get();
    //binding_test().get();

    return test_result();
}

