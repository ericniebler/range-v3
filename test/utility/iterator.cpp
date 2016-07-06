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

#include <vector>
#include <string>
#include <range/v3/utility/iterator.hpp>
#include <range/v3/algorithm/copy.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"

using namespace ranges;

void test_insert_iterator()
{
    CONCEPT_ASSERT(OutputIterator<insert_iterator<std::vector<int>>, int&&>());
    CONCEPT_ASSERT(!EqualityComparable<insert_iterator<std::vector<int>>>());
    std::vector<int> vi{5,6,7,8};
    copy({1,2,3,4}, inserter(vi, vi.begin()+2));
    ::check_equal(vi, {5,6,1,2,3,4,7,8});
}

void test_move_iterator()
{
    std::vector<MoveOnlyString> in;
    in.emplace_back("this");
    in.emplace_back("is");
    in.emplace_back("his");
    in.emplace_back("face");
    std::vector<MoveOnlyString> out;
    auto first = ranges::make_move_iterator(in.begin());
    using I = decltype(first);
    CONCEPT_ASSERT(InputIterator<I>());
    CONCEPT_ASSERT(!ForwardIterator<I>());
    CONCEPT_ASSERT(Same<I, ranges::move_iterator<std::vector<MoveOnlyString>::iterator>>());
    auto last = ranges::make_move_sentinel(in.end());
    using S = decltype(last);
    CONCEPT_ASSERT(Sentinel<S, I>());
    CONCEPT_ASSERT(SizedSentinel<I, I>());
    CHECK((first - first) == 0);
    CONCEPT_ASSERT(SizedSentinel<S, I>());
    CHECK(static_cast<std::size_t>(last - first) == in.size());
    ranges::copy(first, last, ranges::back_inserter(out));
    ::check_equal(in, {"","","",""});
    ::check_equal(out, {"this","is","his","face"});
}

template<class I>
using RI = std::reverse_iterator<I>;

void issue_420_regression()
{
    // Verify that SizedSentinel<std::reverse_iterator<S>, std::reverse_iterator<I>>
    // properly requires SizedSentinel<I, S>
    CONCEPT_ASSERT(SizedSentinel<RI<int*>, RI<int*>>());
    CONCEPT_ASSERT(!SizedSentinel<RI<int*>, RI<float*>>());
    using BI = bidirectional_iterator<int*>;
    CONCEPT_ASSERT(!SizedSentinel<RI<BI>, RI<BI>>());
}

int main()
{
    test_insert_iterator();
    test_move_iterator();
    issue_420_regression();

    return ::test_result();
}
