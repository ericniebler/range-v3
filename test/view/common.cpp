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

#include <list>
#include <vector>
#include <sstream>
#include <range/v3/core.hpp>
#include <range/v3/view/common.hpp>
#include <range/v3/view/counted.hpp>
#include <range/v3/view/delimit.hpp>
#include <range/v3/view/repeat_n.hpp>
#include <range/v3/utility/copy.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"

int main()
{
    using namespace ranges;

    {
        std::stringstream sinx("1 2 3 4 5 6 7 8 9 1 2 3 4 5 6 7 8 9 1 2 3 4 42 6 7 8 9 ");
        auto rng1 = istream<int>(sinx) | view::delimit(42); // | view::common;
        ::models_not<BoundedRangeConcept>(rng1);
        ::models<InputRangeConcept>(rng1);
        ::models_not<ForwardRangeConcept>(rng1);
        auto const& crng1 = rng1;
        ::models_not<RangeConcept>(crng1);
        auto rng2 = rng1 | view::common;
        ::models<BoundedViewConcept>(aux::copy(rng2));
        ::models<InputIteratorConcept>(rng2.begin());
        CPP_assert(Same<typename std::iterator_traits<decltype(rng2.begin())>::iterator_category,
                            std::input_iterator_tag>);
        ::models_not<ForwardIteratorConcept>(rng2.begin());
        ::check_equal(rng2, {1, 2, 3, 4, 5, 6, 7, 8, 9, 1, 2, 3, 4, 5, 6, 7, 8, 9, 1, 2, 3, 4});
    }

    std::vector<int> v{1,2,3,4,5,6,7,8,9,0,42,64};
    {
        auto rng1 = v | view::delimit(42) | view::common;
        ::models<BoundedViewConcept>(aux::copy(rng1));
        ::models_not<SizedViewConcept>(aux::copy(rng1));
        ::models<ForwardIteratorConcept>(rng1.begin());
        ::models_not<BidirectionalIteratorConcept>(rng1.begin());
        auto const & crng1 = rng1;
        auto i = rng1.begin(); // non-const
        auto j = crng1.begin(); // const
        j = i;
        ::check_equal(rng1, {1, 2, 3, 4, 5, 6, 7, 8, 9, 0});
    }

    {
        std::list<int> l{1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6,7,8,9,0};
        auto rng3 = view::counted(l.begin(), 10) | view::common;
        ::models<BoundedViewConcept>(aux::copy(rng3));
        ::models<SizedViewConcept>(aux::copy(rng3));
        ::models<ForwardIteratorConcept>(rng3.begin());
        ::models_not<BidirectionalIteratorConcept>(rng3.begin());
        ::models<SizedSentinelConcept>(rng3.begin(), rng3.end());
        auto b = begin(rng3);
        auto e = end(rng3);
        CHECK((e-b) == 10);
        CHECK((b-e) == -10);
        CHECK((e-e) == 0);
        CHECK((next(b)-b) == 1);

        // Pass-through of already-common ranges is OK:
        rng3 = rng3 | view::common;
    }

    {
        auto rng4 = view::counted(begin(v), 8) | view::common;
        ::models<BoundedViewConcept>(aux::copy(rng4));
        ::models<SizedViewConcept>(aux::copy(rng4));
        ::models<RandomAccessIteratorConcept>(begin(rng4));
        ::check_equal(rng4, {1, 2, 3, 4, 5, 6, 7, 8});
    }

    {
        // Regression test for issue#504:
        auto rng1 = view::repeat_n( 0, 10 );
        ::models_not<BoundedViewConcept>(aux::copy(rng1));
        ::models<RandomAccessViewConcept>(aux::copy(rng1));
        ::models<SizedViewConcept>(aux::copy(rng1));
        auto const& crng1 = rng1;
        ::models<RandomAccessViewConcept>(aux::copy(crng1));
        ::models<SizedViewConcept>(aux::copy(crng1));

        auto rng2 = rng1 | view::common;
        ::models<BoundedViewConcept>(aux::copy(rng2));
        ::models<RandomAccessViewConcept>(aux::copy(rng2));
        ::models<SizedViewConcept>(aux::copy(rng2));
        auto const& crng2 = rng2;
        ::models<BoundedViewConcept>(aux::copy(crng2));
        ::models<RandomAccessViewConcept>(aux::copy(crng2));
        ::models<SizedViewConcept>(aux::copy(crng2));

        ::check_equal(rng2, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0});
    }

    {
        int const data[] = {1,2,3,4};
        auto rng = debug_input_view<int const>{data} | view::common;
        using Rng = decltype(rng);
        CPP_assert(InputView<Rng>);
        CPP_assert(!ForwardRange<Rng>);
        CPP_assert(CommonRange<Rng>);
        ::check_equal(rng, {1,2,3,4});
    }

    return ::test_result();
}
