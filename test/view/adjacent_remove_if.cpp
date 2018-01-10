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

#include <vector>
#include <iterator>
#include <functional>
#include <range/v3/core.hpp>
#include <range/v3/view/adjacent_remove_if.hpp>
#include <range/v3/view/iota.hpp>
#include <range/v3/view/counted.hpp>
#include <range/v3/utility/copy.hpp>
#include <range/v3/algorithm/copy.hpp>
#include <range/v3/utility/iterator.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"
#include "../test_iterators.hpp"

int main()
{
    using namespace ranges;

    int const rgi[] = {1, 1, 1, 2, 3, 4, 4};
    std::vector<int> out;

    auto && rng = rgi | view::adjacent_remove_if(std::equal_to<int>{});
    has_type<int const &>(*begin(rng));
    models<concepts::BoundedView>(aux::copy(rng));
    models_not<concepts::SizedView>(aux::copy(rng));
    models<concepts::BidirectionalIterator>(begin(rng));
    models_not<concepts::RandomAccessIterator>(begin(rng));
    CONCEPT_ASSERT(OutputIterator<decltype(ranges::back_inserter(out)), int>());
    CONCEPT_ASSERT(!EqualityComparable<decltype(ranges::back_inserter(out))>());
    copy(rng, ranges::back_inserter(out));
    ::check_equal(out, {1, 2, 3, 4});

    auto && rng2 = view::counted(rgi, 7)
      | view::adjacent_remove_if([&](int i, int j) { return i == j; });
    has_type<int const &>(*begin(rng2));
    models<concepts::ForwardView>(aux::copy(rng2));
    models<concepts::BoundedView>(aux::copy(rng2));
    models_not<concepts::SizedView>(aux::copy(rng2));
    models<concepts::BidirectionalIterator>(begin(rng2));
    models_not<concepts::RandomAccessIterator>(begin(rng2));
    ::check_equal(rng2, {1, 2, 3, 4});

    auto && rng3 = view::counted(forward_iterator<int const*>(rgi), 7) | view::adjacent_remove_if(std::equal_to<int>{});
    has_type<int const &>(*begin(rng3));
    models<concepts::ForwardView>(aux::copy(rng3));
    models_not<concepts::BoundedView>(aux::copy(rng3));
    models_not<concepts::SizedView>(aux::copy(rng3));
    models<concepts::ForwardIterator>(begin(rng3));
    models_not<concepts::BidirectionalIterator>(begin(rng3));
    ::check_equal(rng3, {1, 2, 3, 4});

    auto && rng4 = view::counted(forward_iterator<int const*>(rgi), 7)
      | view::adjacent_remove_if([](int,int){return true;});
    has_type<int const &>(*begin(rng4));
    CHECK(*begin(rng4) == 4);
    models<concepts::ForwardView>(aux::copy(rng4));
    models_not<concepts::BoundedView>(aux::copy(rng4));
    models_not<concepts::SizedView>(aux::copy(rng4));
    models<concepts::ForwardIterator>(begin(rng4));
    models_not<concepts::BidirectionalIterator>(begin(rng4));
    ::check_equal(rng4, {4});

    auto is_odd_then_even = [](int i, int j){return 1==i%2 && 0 == j%2;};
    auto && rng5 = view::iota(0, 11) | view::adjacent_remove_if(is_odd_then_even);
    has_type<int>(*begin(rng5));
    models<concepts::ForwardView>(aux::copy(rng5));
    models<concepts::BoundedView>(aux::copy(rng5));
    models_not<concepts::SizedView>(aux::copy(rng5));
    models<concepts::BidirectionalIterator>(begin(rng5));
    models_not<concepts::RandomAccessIterator>(begin(rng5));
    ::check_equal(rng5, {0,2,4,6,8,10});

    {
        // Verify that forward and backward traversal both select the same elements.
        auto rng = view::adjacent_remove_if(rgi, std::equal_to<int>{});
        std::vector<int const*> pointers;
        for(auto& i : rng)
            pointers.push_back(&i);
        auto pos = ranges::end(rng);
        for(auto i = pointers.size(); i != 0;)
        {
            CHECK(pos != ranges::begin(rng));
            CHECK(&*--pos == pointers[--i]);
        }
        CHECK(pos == ranges::begin(rng));
    }

    return test_result();
}
