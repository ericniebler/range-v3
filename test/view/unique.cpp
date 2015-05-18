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

#include <range/v3/core.hpp>
#include <range/v3/view/unique.hpp>
#include <range/v3/algorithm/copy.hpp>
#include <range/v3/utility/iterator.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"
#include <vector>

int main()
{
    using namespace ranges;

    int rgi[] = {1, 1, 1, 2, 3, 4, 4};
    std::vector<int> out;

    auto && rng = rgi | view::unique;
    has_type<int &>(*begin(rng));
    models<concepts::BoundedView>(rng);
    models_not<concepts::SizedView>(rng);
    models<concepts::ForwardIterator>(begin(rng));
    models_not<concepts::BidirectionalIterator>(begin(rng));
    copy(rng, ranges::back_inserter(out));
    ::check_equal(out, {1, 2, 3, 4});

    return test_result();
}
