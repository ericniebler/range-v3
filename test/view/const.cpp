// Range v3 library
//
//  Copyright Eric Niebler 2014-present
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
#include <range/v3/view/const.hpp>
#include <range/v3/view/counted.hpp>
#include <range/v3/view/zip.hpp>
#include <range/v3/view/move.hpp>
#include <range/v3/utility/copy.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"
#include "../test_iterators.hpp"

int main()
{
    using namespace ranges;

    int rgi[] = {1, 2, 3, 4};

    auto && rng = rgi | view::const_;
    has_type<int &>(*begin(rgi));
    has_type<int const &>(*begin(rng));
    CONCEPT_ASSERT(Same<range_rvalue_reference_t<decltype(rng)>, int const &&>());
    models<concepts::BoundedView>(aux::copy(rng));
    models<concepts::SizedView>(aux::copy(rng));
    models<concepts::RandomAccessView>(aux::copy(rng));
    ::check_equal(rng, {1, 2, 3, 4});
    CHECK(&*begin(rng) == &rgi[0]);
    CHECK(rng.size() == 4u);

    auto && rng2 = view::counted(forward_iterator<int*>(rgi), 4) | view::const_;
    has_type<int const &>(*begin(rng2));
    CONCEPT_ASSERT(Same<range_rvalue_reference_t<decltype(rng2)>, int const &&>());
    models<concepts::ForwardView>(aux::copy(rng2));
    models_not<concepts::BidirectionalView>(aux::copy(rng2));
    models_not<concepts::BoundedView>(aux::copy(rng2));
    models<concepts::SizedView>(aux::copy(rng2));
    ::check_equal(rng2, {1, 2, 3, 4});
    CHECK(&*begin(rng2) == &rgi[0]);
    CHECK(rng2.size() == 4u);

    auto zip = view::zip(rgi, rgi);
    auto rng3 = zip | view::const_;
    has_type<common_pair<int &, int &>>(*begin(zip));
    has_type<common_pair<int &&, int &&>>(iter_move(begin(zip)));
    has_type<common_pair<int const &, int const &>>(*begin(rng3));
    has_type<common_pair<int const &&, int const &&>>(iter_move(begin(rng3)));
    models<concepts::RandomAccessView>(aux::copy(rng3));
    models<concepts::BoundedView>(aux::copy(rng3));
    models<concepts::SizedView>(aux::copy(rng3));
    using P = std::pair<int,int>;
    ::check_equal(rng3, {P{1,1}, P{2,2}, P{3,3}, P{4,4}});
    CHECK(&(*begin(rng3)).first == &rgi[0]);
    CHECK(rng3.size() == 4u);

    auto zip2 = view::zip(rgi, rgi) | view::move;
    auto rng4 = zip2 | view::const_;
    has_type<common_pair<int &&, int &&>>(*begin(zip2));
    has_type<common_pair<int &&, int &&>>(iter_move(begin(zip2)));
    has_type<common_pair<int const &&, int const &&>>(*begin(rng4));
    has_type<common_pair<int const &&, int const &&>>(iter_move(begin(rng4)));
    models<concepts::RandomAccessView>(aux::copy(rng4));
    models<concepts::BoundedView>(aux::copy(rng4));
    models<concepts::SizedView>(aux::copy(rng4));
    using P = std::pair<int,int>;
    ::check_equal(rng4, {P{1,1}, P{2,2}, P{3,3}, P{4,4}});
    CHECK(&(*begin(rng4)).first == &rgi[0]);
    CHECK(rng4.size() == 4u);

    {
        auto rng = debug_input_view<int>{rgi} | view::const_;
        CONCEPT_ASSERT(Same<int const&, range_reference_t<decltype(rng)>>());
        ::check_equal(rng, rgi);
    }

    return test_result();
}
