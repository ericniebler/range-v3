//  Copyright Eric Niebler 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/

#include <list>
#include <vector>
#include <sstream>
#include <range/v3/core.hpp>
#include <range/v3/view/bounded.hpp>
#include <range/v3/view/delimit.hpp>
#include <range/v3/view/counted.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"

int main()
{
    using namespace ranges;

    std::stringstream sinx("1 2 3 4 5 6 7 8 9 1 2 3 4 5 6 7 8 9 1 2 3 4 42 6 7 8 9 ");
    auto rng0 = istream<int>(sinx) | view::delimit(42) | view::bounded;
    ::models<concepts::BoundedRange>(rng0);
    ::models<concepts::InputIterator>(rng0.begin());
    CONCEPT_ASSERT(Same<typename std::iterator_traits<decltype(rng0.begin())>::iterator_category,
                        std::input_iterator_tag>());
    ::models_not<concepts::ForwardIterator>(rng0.begin());
    ::check_equal(rng0, {1, 2, 3, 4, 5, 6, 7, 8, 9, 1, 2, 3, 4, 5, 6, 7, 8, 9, 1, 2, 3, 4});

    std::vector<int> v{1,2,3,4,5,6,7,8,9,0,42,64};
    auto rng1 = v | view::delimit(42) | view::bounded;
    ::models<concepts::BoundedRange>(rng1);
    ::models_not<concepts::SizedRange>(rng1);
    ::models<concepts::ForwardIterator>(rng1.begin());
    ::models_not<concepts::BidirectionalIterator>(rng1.begin());
    auto const & crng1 = rng1;
    auto i = rng1.begin(); // non-const
    auto j = crng1.begin(); // const
    j = i;
    ::check_equal(rng1, {1, 2, 3, 4, 5, 6, 7, 8, 9, 0});

    std::list<int> l{1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6,7,8,9,0};
    auto rng3 = view::counted(l.begin(), 10) | view::bounded;
    ::models<concepts::BoundedRange>(rng3);
    ::models<concepts::SizedRange>(rng3);
    ::models<concepts::ForwardIterator>(rng3.begin());
    ::models_not<concepts::BidirectionalIterator>(rng3.begin());
    ::models<concepts::SizedIteratorRange>(rng3.begin(), rng3.end());
    auto b = begin(rng3);
    auto e = end(rng3);
    CHECK((e-b) == 10);
    CHECK((b-e) == -10);
    CHECK((e-e) == 0);
    CHECK((next(b)-b) == 1);

    return ::test_result();
}
