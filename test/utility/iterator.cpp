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
    std::vector<std::string> in{"this","is","his","face"};
    std::vector<std::string> out;
    auto it = ranges::make_move_iterator(in.begin());
    using I = decltype(it);
    CONCEPT_ASSERT(Same<I, ranges::move_iterator<std::vector<std::string>::iterator>>());
    ranges::copy(it, ranges::make_move_sentinel(in.end()),
        ranges::back_inserter(out));
    ::check_equal(in, {"","","",""});
    ::check_equal(out, {"this","is","his","face"});
    ::models<concepts::InputIterator>((I&&)it);
    ::models_not<concepts::ForwardIterator>((I&&)it);
}

int main()
{
    test_insert_iterator();
    test_move_iterator();

    return ::test_result();
}
