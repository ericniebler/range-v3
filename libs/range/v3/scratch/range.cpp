// Boost.Range library
//
//  Copyright Eric Niebler 2013.
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//

#include <vector>
#include <iostream>
#include <range/v3/range.hpp>

int main()
{
    using namespace ranges;
    std::vector<int> v {1,2,3,4,5};

    for_each(v, [](int i){ std::cout << i << ' '; });
    std::cout << '\n';

    auto t = view::transform(v, [](int i){return i * i;});
    for_each(t, [](int i){ std::cout << i << ' '; });
    std::cout << '\n';
}
