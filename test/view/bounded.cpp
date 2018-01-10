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
#include <range/v3/view/bounded.hpp>
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
        auto rng1 = istream<int>(sinx) | view::delimit(42); // | view::bounded;
        ::models_not<concepts::BoundedRange>(rng1);
        ::models<concepts::InputRange>(rng1);
        ::models_not<concepts::ForwardRange>(rng1);
        auto const& crng1 = rng1;
        ::models_not<concepts::Range>(crng1);
        auto rng2 = rng1 | view::bounded;
        ::models<concepts::BoundedView>(aux::copy(rng2));
        ::models<concepts::InputIterator>(rng2.begin());
        CONCEPT_ASSERT(Same<typename std::iterator_traits<decltype(rng2.begin())>::iterator_category,
                            std::input_iterator_tag>());
        ::models_not<concepts::ForwardIterator>(rng2.begin());
        ::check_equal(rng2, {1, 2, 3, 4, 5, 6, 7, 8, 9, 1, 2, 3, 4, 5, 6, 7, 8, 9, 1, 2, 3, 4});
    }

    std::vector<int> v{1,2,3,4,5,6,7,8,9,0,42,64};
    {
        auto rng1 = v | view::delimit(42) | view::bounded;
        ::models<concepts::BoundedView>(aux::copy(rng1));
        ::models_not<concepts::SizedView>(aux::copy(rng1));
        ::models<concepts::ForwardIterator>(rng1.begin());
        ::models_not<concepts::BidirectionalIterator>(rng1.begin());
        auto const & crng1 = rng1;
        auto i = rng1.begin(); // non-const
        auto j = crng1.begin(); // const
        j = i;
        ::check_equal(rng1, {1, 2, 3, 4, 5, 6, 7, 8, 9, 0});
    }

    {
        std::list<int> l{1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6,7,8,9,0};
        auto rng3 = view::counted(l.begin(), 10) | view::bounded;
        ::models<concepts::BoundedView>(aux::copy(rng3));
        ::models<concepts::SizedView>(aux::copy(rng3));
        ::models<concepts::ForwardIterator>(rng3.begin());
        ::models_not<concepts::BidirectionalIterator>(rng3.begin());
        ::models<concepts::SizedSentinel>(rng3.begin(), rng3.end());
        auto b = begin(rng3);
        auto e = end(rng3);
        CHECK((e-b) == 10);
        CHECK((b-e) == -10);
        CHECK((e-e) == 0);
        CHECK((next(b)-b) == 1);

        // Pass-through of already-bounded ranges is OK:
        rng3 = rng3 | view::bounded;
    }

    {
        auto rng4 = view::counted(begin(v), 8) | view::bounded;
        ::models<concepts::BoundedView>(aux::copy(rng4));
        ::models<concepts::SizedView>(aux::copy(rng4));
        ::models<concepts::RandomAccessIterator>(begin(rng4));
        ::check_equal(rng4, {1, 2, 3, 4, 5, 6, 7, 8});
    }

    {
        // Regression test for issue#504:
        auto rng1 = view::repeat_n( 0, 10 );
        ::models_not<concepts::BoundedView>(aux::copy(rng1));
        ::models<concepts::RandomAccessView>(aux::copy(rng1));
        ::models<concepts::SizedView>(aux::copy(rng1));
        auto const& crng1 = rng1;
        ::models<concepts::RandomAccessView>(aux::copy(crng1));
        ::models<concepts::SizedView>(aux::copy(crng1));

        auto rng2 = rng1 | view::bounded;
        ::models<concepts::BoundedView>(aux::copy(rng2));
        ::models<concepts::RandomAccessView>(aux::copy(rng2));
        ::models<concepts::SizedView>(aux::copy(rng2));
        auto const& crng2 = rng2;
        ::models<concepts::BoundedView>(aux::copy(crng2));
        ::models<concepts::RandomAccessView>(aux::copy(crng2));
        ::models<concepts::SizedView>(aux::copy(crng2));

        ::check_equal(rng2, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0});
    }

    {
        int const data[] = {1,2,3,4};
        auto rng = debug_input_view<int const>{data} | view::bounded;
        using Rng = decltype(rng);
        CONCEPT_ASSERT(InputView<Rng>());
        CONCEPT_ASSERT(!ForwardRange<Rng>());
        CONCEPT_ASSERT(BoundedRange<Rng>());
        ::check_equal(rng, {1,2,3,4});
    }

    return ::test_result();
}
