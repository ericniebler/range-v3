// Range v3 library
//
//  Copyright Mitsutaka Takeda 2018-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#include <vector>
#include <range/v3/view/exclusive_scan.hpp>
#include "../test_utils.hpp"

int main()
{
    using namespace ranges;

    {// For non empty range.
        int rgi[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

        {
            auto rng = rgi | view::exclusive_scan(0);
            has_type<int &>(*begin(rgi));
            has_type<int>(*begin(rng));
            models<concepts::SizedView>(aux::copy(rng));
            models<concepts::ForwardView>(aux::copy(rng));
            models_not<concepts::BidirectionalView>(aux::copy(rng));
            ::check_equal(rng, {0, 1, 3, 6, 10, 15, 21, 28, 36, 45});
        }

        {// Test exclusive_scan with a mutable lambda
            int cnt = 0;
            auto mutable_rng = view::exclusive_scan(rgi, 0, [cnt](int i, int j) mutable {return i + j + cnt++;});
            ::check_equal(mutable_rng, {0, 1, 4, 9, 16, 25, 36, 49, 64, 81});
            CONCEPT_ASSERT(View<decltype(mutable_rng)>());
            CONCEPT_ASSERT(!View<decltype(mutable_rng) const>());
        }
    }

    {// For an empty range.
        std::vector<int> rgi;
        auto rng = rgi | view::exclusive_scan(0);
        has_type<int>(*begin(rng));
        models<concepts::SizedView>(aux::copy(rng));
        models<concepts::ForwardView>(aux::copy(rng));
        models_not<concepts::BidirectionalView>(aux::copy(rng));
        CHECK(empty(rng));
    }

    return test_result();
}
