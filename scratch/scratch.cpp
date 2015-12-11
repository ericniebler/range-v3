// Range v3 library
//
//  Copyright Eric Niebler 2013-2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#include <iostream>
#include <range/v3/all.hpp>

using namespace ranges;

int main()
{
    std::vector<int> rgi{1,2,3,4,5,6,7,8};
    RANGES_FOR(int i, rgi | view::filter([](int i){return i%2==0;}))
        std::cout << i << std::endl;
}
