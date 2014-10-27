// Range v3 library
//
//  Copyright Eric Niebler 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <array>
#include <vector>
#include <range/v3/core.hpp>
#include <range/v3/container/concepts.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"

int main()
{
    using namespace ranges;

    int rgi[6];
    ::models<concepts::Iterable>(rgi);
    ::models_not<concepts::SemiContainer>(rgi);

    std::array<int, 6> a;
    ::models<concepts::SemiContainer>(a);
    ::models_not<concepts::Container>(a);

    std::vector<int> v;
    ::models<concepts::Container>(v);

    return ::test_result();
}
