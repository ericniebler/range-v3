//  Copyright Eric Niebler 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/

#include <vector>
#include <sstream>
#include <range/v3/core.hpp>
#include <range/v3/view/as_bounded_range.hpp>
#include <range/v3/view/delimit.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"

int main()
{
    using namespace ranges;

    std::stringstream sinx("1 2 3 4 5 6 7 8 9 1 2 3 4 5 6 7 8 9 1 2 3 4 42 6 7 8 9 ");
    auto rng0 = istream<int>(sinx) | view::delimit(42) | view::as_bounded_range;
    ::models<concepts::BoundedRange>(rng0);
    ::models<concepts::InputIterator>(rng0.begin());
    CONCEPT_ASSERT(Same<typename std::iterator_traits<decltype(rng0.begin())>::iterator_category,
                        std::input_iterator_tag>());
    ::models_not<concepts::ForwardIterator>(rng0.begin());
    ::check_equal(rng0, {1, 2, 3, 4, 5, 6, 7, 8, 9, 1, 2, 3, 4, 5, 6, 7, 8, 9, 1, 2, 3, 4});

    std::vector<int> v;
    auto rng1 = v | view::delimit(42) | view::as_bounded_range;
    ::models<concepts::BoundedRange>(rng1);
    ::models<concepts::RandomAccessIterator>(rng1.begin());
    auto const & crng1 = rng1;
    auto i = rng1.begin(); // non-const
    auto j = crng1.begin(); // const
    j = i;

    return ::test_result();
}
