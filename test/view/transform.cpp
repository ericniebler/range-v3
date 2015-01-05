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

#include <iterator>
#include <functional>
#include <range/v3/core.hpp>
#include <range/v3/view/transform.hpp>
#include <range/v3/view/counted.hpp>
#include <range/v3/view/reverse.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"

struct is_odd
{
    bool operator()(int i) const
    {
        return (i % 2) == 1;
    }
};

int main()
{
    using namespace ranges;

    int rgi[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    auto && rng = rgi | view::transform(is_odd());
    has_type<int &>(*begin(rgi));
    has_type<bool>(*begin(rng));
    models<concepts::SizedRange>(rng);
    models<concepts::RandomAccessRange>(rng);
    ::check_equal(rng, {true, false, true, false, true, false, true, false, true, false});

    std::pair<int, int> rgp[] = {{1,1}, {2,2}, {3,3}, {4,4}, {5,5}, {6,6}, {7,7}, {8,8}, {9,9}, {10,10}};
    auto && rng2 = rgp | view::transform(&std::pair<int,int>::first);
    has_type<int &>(*begin(rgi));
    has_type<int &>(*begin(rng2));
    models<concepts::BoundedRange>(rng2);
    models<concepts::SizedRange>(rng2);
    models<concepts::RandomAccessRange>(rng2);
    ::check_equal(rng2, {1,2,3,4,5,6,7,8,9,10});
    ::check_equal(rng2 | view::reverse, {10,9,8,7,6,5,4,3,2,1});
    CHECK(&*begin(rng2) == &rgp[0].first);
    CHECK(rng2.size() == 10u);

    auto && rng3 = view::counted(rgp, 10) | view::transform(&std::pair<int,int>::first);
    has_type<int &>(*begin(rgi));
    has_type<int &>(*begin(rng3));
    models<concepts::BoundedRange>(rng3);
    models<concepts::SizedRange>(rng3);
    models<concepts::RandomAccessRange>(rng3);
    ::check_equal(rng3, {1,2,3,4,5,6,7,8,9,10});
    CHECK(&*begin(rng3) == &rgp[0].first);
    CHECK(rng3.size() == 10u);

    auto && rng4 = view::counted(forward_iterator<std::pair<int, int>*>{rgp}, 10)
                      | view::transform(&std::pair<int,int>::first);
    has_type<int &>(*begin(rgi));
    has_type<int &>(*begin(rng4));
    models_not<concepts::BoundedRange>(rng4);
    models<concepts::SizedRange>(rng4);
    models<concepts::ForwardRange>(rng4);
    models_not<concepts::BidirectionalRange>(rng4);
    ::check_equal(rng4, {1,2,3,4,5,6,7,8,9,10});
    CHECK(&*begin(rng4) == &rgp[0].first);
    CHECK(rng4.size() == 10u);

    counted_iterator<forward_iterator<std::pair<int, int>*>> i = begin(rng4).base();
    (void)i;

    // Test transform with a mutable lambda
    int cnt = 100;
    auto mutable_rng = view::transform(rgi, [cnt](int i) mutable { return cnt++;});
    ::check_equal(mutable_rng, {100,101,102,103,104,105,106,107,108,109});
    CHECK(cnt == 100);
    CONCEPT_ASSERT(Range<decltype(mutable_rng)>());
    CONCEPT_ASSERT(!Range<decltype(mutable_rng) const>());

    return test_result();
}
