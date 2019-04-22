// Range v3 library
//
//  Copyright Johel Guerrero 2019
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#include <concepts/concepts.hpp>
#include <range/v3/iterator/operations.hpp>
#include <range/v3/range/operations.hpp>
#include <range/v3/range/primitives.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/utility/copy.hpp>
#include <range/v3/view/addressof.hpp>
#include <range/v3/view/drop.hpp>
#include <range/v3/view/drop_while.hpp>
#include <range/v3/view/reverse.hpp>
#include <range/v3/view/subrange.hpp>
#include <range/v3/view/tail.hpp>
#include <range/v3/view/trim.hpp>

#include "../test_iterators.hpp"
#include "../test_utils.hpp"

int main()
{
    using namespace ranges;
    int ia[] = {0, 1, 2, 3, 4, 3, 2, 1, 2, 3, 4, 3, 2, 1, 0};
    int ib[] =             {4, 3, 2, 1, 2, 3, 4};
    constexpr auto bs = distance(ib);
    auto p = [](int i) { return i < 4; };

    auto && rng = view::trim(ia, p);
    static_assert(Same<iterator_t<decltype(rng)>, iterator_t<decltype(ia)>>, "");
    models<ContiguousViewConcept>(aux::copy(rng));
    models<CommonRangeConcept>(aux::copy(rng));
    models_not<SizedRangeConcept>(aux::copy(rng));
    check_equal(rng, ib);

    auto && rng2 = view::trim(ib, p);
    check_equal(view::addressof(rng2), view::addressof(ib));

    auto && rng3 = ia | view::drop(4) | view::trim(p);
    static_assert(Same<iterator_t<decltype(rng3)>, iterator_t<decltype(rng3.base())>>, "");
    models<ContiguousViewConcept>(aux::copy(rng3));
    models<CommonRangeConcept>(aux::copy(rng3));
    models_not<SizedRangeConcept>(aux::copy(rng3));
    check_equal(rng3, ib);

    auto && rng4 = ia | view::reverse | view::drop(4) | view::trim(p);
    static_assert(Same<iterator_t<decltype(rng4)>, iterator_t<decltype(rng4.base())>>, "");
    models<RandomAccessViewConcept>(aux::copy(rng4));
    models<CommonRangeConcept>(aux::copy(rng4));
    models_not<SizedRangeConcept>(aux::copy(rng4));
    check_equal(rng4, ib);

    check_equal(view::trim(ia, p), ia | view::drop_while(p) | view::reverse | view::drop_while(p));

    auto && rng5 = make_subrange(ib + 1, ib + bs - 1) | view::trim(p);
    static_assert(Same<iterator_t<decltype(rng5)>, iterator_t<decltype(rng5.base())>>, "");
    models<ContiguousViewConcept>(aux::copy(rng5));
    models<CommonRangeConcept>(aux::copy(rng5));
    models_not<SizedRangeConcept>(aux::copy(rng5));
    CHECK(empty(rng5));

    auto && rng6 = make_subrange(ib, ib + bs - 1) | view::trim(p);
    CHECK(distance(rng6) == 1);
    check_equal(&front(rng6), ib);

    auto && rng7 = ib | view::tail | view::trim(p);
    static_assert(Same<iterator_t<decltype(rng7)>, iterator_t<decltype(rng7.base())>>, "");
    models<ContiguousViewConcept>(aux::copy(rng7));
    models<CommonRangeConcept>(aux::copy(rng7));
    models_not<SizedRangeConcept>(aux::copy(rng7));
    CHECK(distance(rng7) == 1);
    check_equal(&front(rng7), ib + bs - 1);

    auto && rng8 = make_subrange(bidirectional_iterator<const int*>(ia),
        bidirectional_iterator<const int*>(ia + distance(ia))) | view::trim(p);
    static_assert(Same<iterator_t<decltype(rng8)>, iterator_t<decltype(rng8.base())>>, "");
    models<BidirectionalViewConcept>(aux::copy(rng8));
    models<CommonRangeConcept>(aux::copy(rng8));
    models_not<SizedRangeConcept>(aux::copy(rng8));

    return test_result();
}
