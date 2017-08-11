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

#include <list>
#include <vector>
#include <sstream>
#include <range/v3/core.hpp>
#include <range/v3/istream_range.hpp>
#include <range/v3/view/move.hpp>
#include <range/v3/view/reverse.hpp>
#include <range/v3/view/stride.hpp>
#include <range/v3/algorithm/copy.hpp>
#include <range/v3/utility/counted_iterator.hpp>
#include <range/v3/utility/iterator.hpp>
#include <range/v3/numeric.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"
#include "../test_iterators.hpp"

int main()
{
    using namespace ranges;

    std::vector<int> v(50);
    iota(v, 0);

    if (!ranges::v3::detail::broken_ebo)
        CHECK(
            sizeof((v | view::stride(3)).begin()) ==
            sizeof(void*) + sizeof(v.begin()) + sizeof(std::ptrdiff_t));
    ::check_equal(v | view::stride(3) | view::reverse,
                  {48, 45, 42, 39, 36, 33, 30, 27, 24, 21, 18, 15, 12, 9, 6, 3, 0});

    std::stringstream str;
    copy(v, ostream_iterator<int>{str, " "});
    auto x = istream<int>(str) | view::stride(3);
    CONCEPT_ASSERT(InputView<decltype(x)>());
    CONCEPT_ASSERT(!InputView<decltype(x) const>());
    CONCEPT_ASSERT(!ForwardView<decltype(x)>());
    check_equal(x, {0, 3, 6, 9, 12, 15, 18, 21, 24, 27, 30, 33, 36, 39, 42, 45, 48});

    std::list<int> li;
    copy(v, back_inserter(li));
    ::check_equal(li | view::stride(3),
                  {0, 3, 6, 9, 12, 15, 18, 21, 24, 27, 30, 33, 36, 39, 42, 45, 48});
    ::check_equal(li | view::stride(3) | view::reverse,
                  {48, 45, 42, 39, 36, 33, 30, 27, 24, 21, 18, 15, 12, 9, 6, 3, 0});

    for(int i : li | view::stride(3) | view::reverse)
        std::cout << i << ' ';
    std::cout << '\n';

    auto x2 = v | view::stride(3);
    CHECK(ranges::distance(x2) == 17);

    auto it0 = x2.begin();
    auto it1 = std::next(it0, 10);
    CHECK((it1 - it0) == 10);
    CHECK((it0 - it1) == -10);
    CHECK((it0 - it0) == 0);
    CHECK((it1 - it1) == 0);

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
        ::check_equal(rng, {0,2,4,6});
    }

    return ::test_result();
}
