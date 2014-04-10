//  Copyright Eric Niebler 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/

#include <range/v3/core.hpp>
#include <range/v3/view/counted.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"

int main()
{
    using namespace ranges;
    std::cout << "\nTesting counted\n";
    int rgi[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    auto rng = view::counted(rgi, 10);
    rng.size();
    CONCEPT_ASSERT(SizedIterable<decltype(rng)>());
    auto i = rng.begin();
    auto b = i.base();
    auto c = i.count();
    decltype(i) j{b, c};
    ::check_equal(rng, {1, 2, 3, 4, 5, 6, 7, 8, 9, 10});
    static_assert(std::is_same<decltype(i), counted_iterator<int*>>::value, "");
}
