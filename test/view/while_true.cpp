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
#include <range/v3/view/while_true.hpp>
#include <range/v3/algorithm/for_each.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"

int main()
{
    using namespace ranges;
    // TODO: if lambda is passed to while_true, rng0 does not model the Iterable
    // concept anymore
    std::function<bool(int)> pred = [](auto i) { return i != 25; };
    auto rng0 = view::iota(10) | view::while_true(pred);
    ::check_equal(rng0, {10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24});
    ::models<concepts::Iterable>(rng0);
    ::models_not<concepts::Range>(rng0);
    ::models<concepts::RandomAccessIterator>(rng0.begin());
    CONCEPT_ASSERT(RandomAccessIterable<while_trueed_view<std::vector<int>, std::function<bool(int)>>>());

    std::vector<int> vi{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    auto rng1 = vi | view::while_true([](auto i) { return i != 50; });
    ::check_equal(rng1, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9});

    return test_result();
}
