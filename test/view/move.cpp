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
#include <range/v3/iterator/operations.hpp>
#include <range/v3/utility/copy.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"

int main()
{
    using namespace ranges;
    static const char * const data[] = {"'allo", "'allo", "???"};
    std::vector<MoveOnlyString> vs(begin(data), end(data));

    auto x = vs | view::move;
    CPP_assert(Same<common_view_tag_of<decltype(x)>, common_view_tag>);
    CPP_assert(Same<sized_view_tag_of<decltype(x)>, sized_view_tag>);
    ::models<BoundedViewConcept>(aux::copy(x));
    ::models<SizedViewConcept>(aux::copy(x));
    ::models<RandomAccessIteratorConcept>(x.begin());
    using I = decltype(x.begin());
    CPP_assert(Same<iterator_tag_of<I>, ranges::detail::random_access_iterator_tag_>);
    CPP_assert(Same<
        typename std::iterator_traits<I>::iterator_category,
        std::random_access_iterator_tag>);

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
