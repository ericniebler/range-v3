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
#include <range/v3/istream_range.hpp>
#include <range/v3/view/move.hpp>
#include <range/v3/view/reverse.hpp>
#include <range/v3/view/stride.hpp>
#include <range/v3/view/iota.hpp>
#include <range/v3/algorithm/copy.hpp>
#include <range/v3/utility/iterator.hpp>
#include <range/v3/numeric.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"
#include "../test_iterators.hpp"

int main()
{
    using namespace ranges;

    auto const v = []
    {
        std::vector<int> v(50);
        iota(v, 0);
        return v;
    }();
    {
        auto rng = v | view::stride(3);
        using R = decltype(rng);
        CONCEPT_ASSERT(RandomAccessView<R>());
        CONCEPT_ASSERT(!ContiguousRange<R>());
        CONCEPT_ASSERT(BoundedRange<R>());
        CONCEPT_ASSERT(SizedRange<R>());
        CONCEPT_ASSERT(Range<R const>());
        ::check_equal(rng | view::reverse,
                    {48, 45, 42, 39, 36, 33, 30, 27, 24, 21, 18, 15, 12, 9, 6, 3, 0});
    }

    {
        std::stringstream str;
        copy(v, ostream_iterator<int>{str, " "});
        auto rng = istream<int>(str) | view::stride(3);
        using R = decltype(rng);
        CONCEPT_ASSERT(InputView<R>());
        CONCEPT_ASSERT(!ForwardRange<R>());
        CONCEPT_ASSERT(!BoundedRange<R>());
        CONCEPT_ASSERT(!SizedRange<R>());
        CONCEPT_ASSERT(!Range<R const>());
        check_equal(rng, {0, 3, 6, 9, 12, 15, 18, 21, 24, 27, 30, 33, 36, 39, 42, 45, 48});
    }

    {
        std::list<int> li;
        copy(v, back_inserter(li));
        auto rng = li | view::stride(3);
        using R = decltype(rng);
        CONCEPT_ASSERT(BidirectionalView<R>());
        CONCEPT_ASSERT(!RandomAccessRange<R>());
        CONCEPT_ASSERT(BoundedRange<R>());
        CONCEPT_ASSERT(SizedRange<R>());
        CONCEPT_ASSERT(Range<R const>());
        ::check_equal(rng,
                    {0, 3, 6, 9, 12, 15, 18, 21, 24, 27, 30, 33, 36, 39, 42, 45, 48});
        ::check_equal(rng | view::reverse,
                    {48, 45, 42, 39, 36, 33, 30, 27, 24, 21, 18, 15, 12, 9, 6, 3, 0});

        for(int i : rng | view::reverse)
            std::cout << i << ' ';
        std::cout << '\n';
    }

    {
        auto x2 = v | view::stride(3);
        CHECK(ranges::distance(x2) == 17);

        auto it0 = x2.begin();
        auto it1 = std::next(it0, 10);
        CHECK((it1 - it0) == 10);
        CHECK((it0 - it1) == -10);
        CHECK((it0 - it0) == 0);
        CHECK((it1 - it1) == 0);
    }

    {
        const auto n = 4;
        auto rng = v | view::move | view::stride(2);
        CHECK((next(begin(rng), n) - begin(rng)) == n);
    }

    {
        // Regression test #368
        int n = 42;
        (void)ranges::view::stride(n);
    }

    {
        int const some_ints[] = {0,1,2,3,4,5,6,7};
        auto rng = debug_input_view<int const>{some_ints} | view::stride(2);
        using R = decltype(rng);
        CONCEPT_ASSERT(InputView<R>());
        CONCEPT_ASSERT(!ForwardRange<R>());
        CONCEPT_ASSERT(!BoundedRange<R>());
        CONCEPT_ASSERT(SizedRange<R>());
        CONCEPT_ASSERT(!Range<R const>());
        ::check_equal(rng, {0,2,4,6});
    }

    {
        std::list<int> li;
        copy(v, back_inserter(li));
        iterator_range<std::list<int>::const_iterator> tmp{li.begin(), li.end()};
        auto rng = tmp | view::stride(3);
        using R = decltype(rng);
        CONCEPT_ASSERT(BidirectionalView<R>());
        CONCEPT_ASSERT(!RandomAccessRange<R>());
        CONCEPT_ASSERT(!BoundedRange<R>());
        CONCEPT_ASSERT(!SizedRange<R>());
        CONCEPT_ASSERT(!Range<R const>());
        ::check_equal(rng,
                    {0, 3, 6, 9, 12, 15, 18, 21, 24, 27, 30, 33, 36, 39, 42, 45, 48});
        ::check_equal(rng | view::reverse,
                    {48, 45, 42, 39, 36, 33, 30, 27, 24, 21, 18, 15, 12, 9, 6, 3, 0});
    }

    {
        std::list<int> li;
        copy(v, back_inserter(li));
        sized_iterator_range<std::list<int>::const_iterator> tmp{li.begin(), li.end(), li.size()};
        auto rng = tmp | view::stride(3);
        using R = decltype(rng);
        CONCEPT_ASSERT(BidirectionalView<R>());
        CONCEPT_ASSERT(!RandomAccessRange<R>());
        CONCEPT_ASSERT(BoundedRange<R>());
        CONCEPT_ASSERT(SizedRange<R>());
        CONCEPT_ASSERT(Range<R const>());
        CHECK((*--rng.end()) == 48);
        ::check_equal(rng,
                    {0, 3, 6, 9, 12, 15, 18, 21, 24, 27, 30, 33, 36, 39, 42, 45, 48});
        ::check_equal(rng | view::reverse,
                    {48, 45, 42, 39, 36, 33, 30, 27, 24, 21, 18, 15, 12, 9, 6, 3, 0});
    }

    // https://github.com/ericniebler/range-v3/issues/901
    {
        auto r = view::iota( 0, 12 );

        // Evenly divisible stride:
        auto strided1 = r | view::stride(3);
        ::check_equal(strided1, {0, 3, 6, 9});
        CHECK(strided1.size() == 4u);
        CHECK(strided1.front() == 0);
        CHECK(strided1[0] == 0);
        CHECK(strided1.back() == 9);
        CHECK(strided1[3] == 9);
        CHECK(strided1[(int)strided1.size() - 1] == 9);

        // Not evenly divisible stride:
        auto strided2 = r | view::stride(5);
        ::check_equal(strided2, {0, 5, 10});
        CHECK(strided2.size() == 3u);
        CHECK(strided2.front() == 0);
        CHECK(strided2[0] == 0);
        CHECK(strided2.back() == 10);
        CHECK(strided2[2] == 10);
        CHECK(strided2[(int)strided2.size() - 1] == 10);
    }

    return ::test_result();
}
