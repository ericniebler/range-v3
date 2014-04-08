//  Copyright Eric Niebler 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/

#include <range/v3/core.hpp>
#include <range/v3/view/unique.hpp>
#include <range/v3/algorithm/copy.hpp>
#include <range/v3/utility/iterator.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"

int main()
{
    using namespace ranges;

    int rgi[] = {1, 1, 1, 2, 3, 4, 4};
    std::vector<int> out;

    auto && rng = rgi | view::unique;
    has_type<int &>(*begin(rng));
    models<concepts::Range>(rng);
    models_not<concepts::SizedRange>(rng);
    models<concepts::ForwardIterator>(begin(rng));
    models_not<concepts::BidirectionalIterator>(begin(rng));
    copy(rng, ranges::back_inserter(out));
    CHECK(range_equal(out, {1, 2, 3, 4}));

    return test_result();
}
