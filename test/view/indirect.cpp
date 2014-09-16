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
#include <memory>
#include <range/v3/core.hpp>
#include <range/v3/view/indirect.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"

int main()
{
    using namespace ranges;
    std::vector<std::shared_ptr<int>> vp;
    for(int i = 0; i < 10; ++i)
        vp.push_back(std::make_shared<int>(i));
    auto && rng = vp | view::indirect;
    CHECK(&*begin(rng) == vp[0].get());
    ::check_equal(rng, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9});

    return test_result();
}
