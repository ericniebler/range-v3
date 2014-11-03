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
#include <range/v3/algorithm/move.hpp>
#include <range/v3/container/sort.hpp>
#include <range/v3/container/erase_if.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"

int main()
{
    using namespace ranges;

    std::vector<int> v = view::ints(1,20);
    auto & v2 = cont::erase_if(v, [](int i){return i % 2 == 0;});
    CHECK(&v2 == &v);
    check_equal(v, {1,3,5,7,9,11,13,15,17,19});

    //std::function<bool(int)> f = std::bind(std::less<int>{}, std::placeholders::_1, 10);
    //auto && v3 = v | move | cont::erase_if(f);
    auto && v3 = v | move | cont::erase_if(std::bind(std::less<int>{}, std::placeholders::_1, 10));
    //auto && v3 = v | move | cont::erase_if([](int i){return false;});
    check_equal(v3, {11,13,15,17,19});



    return ::test_result();
}
