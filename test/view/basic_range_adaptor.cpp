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

#include <range/v3/detail/config.hpp>

#if RANGES_CXX_VER >= RANGES_CXX_STD_17

#include <vector>
#include <sstream>

#include <range/v3/view/basic_range_adaptor.hpp>
#include <range/v3/view/transform.hpp>
#include <range/v3/view/drop.hpp>

#include "../simple_test.hpp"
#include "../test_utils.hpp"

using namespace ranges;

int main()
{
    std::cout << "basic_range_adaptor exists. Start tests. " << std::endl;

    std::vector<int> vec = {1,2,3,4};

    check_equal(
        vec | view::basic_range_adaptor<drop_view>(1),
        vec | view::drop(1)
    );

    auto fn = [](int i) { return i+10; };
    check_equal(
        vec | view::basic_range_adaptor<transform_view>(fn),
        vec | view::transform(fn)
    );

    return test_result();
}

#else

int main()
{
    std::cout << "basic_range_adaptor does not exists. Skip tests. " << RANGES_CXX_VER << std::endl;
    return 0;
}

#endif // RANGES_CXX_VER >= RANGES_CXX_STD_17

