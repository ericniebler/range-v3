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

#include <cstring>
#include <string>
#include <vector>
#include <range/v3/core.hpp>
#include <range/v3/view/move.hpp>
#include <range/v3/algorithm/copy.hpp>
#include <range/v3/utility/iterator.hpp>
#include <range/v3/utility/copy.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"

int main()
{
    using namespace ranges;
    static const char * const data[] = {"'allo", "'allo", "???"};
    std::vector<MoveOnlyString> vs(begin(data), end(data));

    auto x = vs | view::move;
    CONCEPT_ASSERT(Same<bounded_view_concept_t<decltype(x)>, concepts::BoundedView>());
    CONCEPT_ASSERT(Same<sized_view_concept_t<decltype(x)>, concepts::SizedView>());
    ::models<concepts::BoundedView>(aux::copy(x));
    ::models<concepts::SizedView>(aux::copy(x));
    ::models<concepts::RandomAccessIterator>(x.begin());
    using I = decltype(x.begin());
    CONCEPT_ASSERT(Same<iterator_concept_t<I>, concepts::RandomAccessIterator>());
    CONCEPT_ASSERT(Same<iterator_category_t<I>, ranges::random_access_iterator_tag>());

    CHECK(bool(*x.begin() == "'allo"));

    std::vector<MoveOnlyString> vs2(x.begin(), x.end());
    static_assert(std::is_same<MoveOnlyString&&, decltype(*x.begin())>::value, "");
    ::check_equal(vs2, {"'allo", "'allo", "???"});
    ::check_equal(vs, {"", "", ""});

    {
        MoveOnlyString data[] = {"can", "you", "hear", "me", "now?"};
        auto rng = debug_input_view<MoveOnlyString>{data} | view::move;
        MoveOnlyString target[sizeof(data) / sizeof(data[0])];
        copy(rng, target);
        ::check_equal(data, {"", "", "", "", ""});
        ::check_equal(target, {"can", "you", "hear", "me", "now?"});
    }

    return test_result();
}
