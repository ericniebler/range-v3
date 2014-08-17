//  Copyright Eric Niebler 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/

#include <vector>
#include <range/v3/core.hpp>
#include <range/v3/view/iota.hpp>
#include <range/v3/view/take_while.hpp>
#include <range/v3/algorithm/for_each.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"

int main()
{
    using namespace ranges;
    auto rng0 = view::iota(10) | view::take_while([](int i) { return i != 25; });
    ::check_equal(rng0, {10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24});
    ::models<concepts::Range>(rng0);
    ::models_not<concepts::BoundedRange>(rng0);
    ::models<concepts::RandomAccessIterator>(rng0.begin());
    CONCEPT_ASSERT(RandomAccessRange<take_while_view<std::vector<int>, std::function<bool(int)>>>());

    std::vector<int> vi{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    auto rng1 = vi | view::take_while([](int i) { return i != 50; });
    ::check_equal(rng1, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9});

    return test_result();
}
