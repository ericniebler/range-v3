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

#include <string>
#include <sstream>
#include <vector>
#include <range/v3/core.hpp>
#include <range/v3/istream_range.hpp>
#include <range/v3/view/replace_if.hpp>
#include <range/v3/view/bounded.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"

int main()
{
    using namespace ranges;
    using namespace ranges;
    using namespace std::placeholders;

    std::string str{"1 2 3 4 5 6 7 8 9 1 2 3 4 5 6 7 8 9 1 2 3 4 5 6 7 8 9 "};
    std::stringstream sin{str};

    auto && rng = istream<int>(sin) | view::replace_if([](int i){return i == 1; }, 42);
    has_type<int>(*begin(rng));
    models<concepts::Range>(rng);
    models_not<concepts::SizedRange>(rng);
    models_not<concepts::BoundedRange>(rng);
    models<concepts::InputIterator>(begin(rng));
    models_not<concepts::ForwardIterator>(begin(rng));

    auto && tmp = rng | view::bounded;
    has_type<int>(*begin(tmp));
    models<concepts::BoundedRange>(tmp);
    models_not<concepts::SizedRange>(tmp);
    models<concepts::InputIterator>(begin(tmp));
    models_not<concepts::ForwardIterator>(begin(tmp));
    std::vector<int> actual{begin(tmp), end(tmp)};
    ::check_equal(actual, {42, 2, 3, 4, 5, 6, 7, 8, 9, 42, 2, 3, 4, 5, 6, 7, 8, 9, 42, 2, 3, 4, 5, 6, 7, 8, 9});

    // Check with a mutable predicate
    int rgi[] = {0,1,2,3,4,5,6,7,8,9};
    bool flag = false;
    auto mutable_only = view::replace_if(rgi, [flag](int) mutable { return flag = !flag;}, 42);
    ::check_equal(mutable_only, {42,1,42,3,42,5,42,7,42,9});
    CONCEPT_ASSERT(Range<decltype(mutable_only)>());
    CONCEPT_ASSERT(!Range<decltype(mutable_only) const>());

    return test_result();
}
