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

#include <iterator>
#include <functional>
#include <range/v3/core.hpp>
#include <range/v3/view/partial_sum.hpp>
#include <range/v3/view/counted.hpp>
#include <range/v3/view/reverse.hpp>
#include <range/v3/utility/copy.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"

struct is_odd
{
    bool operator()(int i) const
    {
        return (i % 2) == 1;
    }
};

int main()
{
    using namespace ranges;

    int rgi[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    auto && rng = rgi | view::partial_sum();
    has_type<int &>(*begin(rgi));
    has_type<int>(*begin(rng));
    models<concepts::SizedView>(aux::copy(rng));
    models<concepts::ForwardView>(aux::copy(rng));
    models_not<concepts::BidirectionalView>(aux::copy(rng));
    ::check_equal(rng, {1, 3, 6, 10, 15, 21, 28, 36, 45, 55});

    auto it = begin(rng);
    CHECK(*it == 1);
    auto it2 = next(it);
    CHECK(*it == 1);
    CHECK(*it2 == 3);
    it2 = it;
    CHECK(*it2 == 1);
    ++it2;
    CHECK(*it2 == 3);

    // Test partial_sum with a mutable lambda
    int cnt = 0;
    auto mutable_rng = view::partial_sum(rgi, [cnt](int i, int j) mutable { return i + j + cnt++;});
    ::check_equal(mutable_rng, {1, 3, 7, 13, 21, 31, 43, 57, 73, 91});
    CHECK(cnt == 0);
    CONCEPT_ASSERT(View<decltype(mutable_rng)>());
    CONCEPT_ASSERT(!View<decltype(mutable_rng) const>());

    return test_result();
}
