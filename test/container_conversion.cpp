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

#include <map>
#include <set>
#include <list>
#include <sstream>
#include <vector>
#include <range/v3/core.hpp>
#include <range/v3/view/take.hpp>
#include <range/v3/view/reverse.hpp>
#include <range/v3/view/transform.hpp>
#include <range/v3/view/for_each.hpp>
#include <range/v3/view/iota.hpp>
#include <range/v3/view/zip.hpp>
#include "./simple_test.hpp"
#include "./test_utils.hpp"

int main()
{
    using namespace ranges;

    std::vector<int> v = view::ints | view::take(10);
    ::check_equal(v, {0,1,2,3,4,5,6,7,8,9});

    v = view::ints(10) | view::take(10) | view::reverse;
    ::check_equal(v, {19,18,17,16,15,14,13,12,11,10});

    std::list<int> l = view::ints | view::take(10);
    ::check_equal(l, {0,1,2,3,4,5,6,7,8,9});

    l = view::ints(10) | view::take(10) | view::reverse;
    ::check_equal(l, {19,18,17,16,15,14,13,12,11,10});

    auto to_string = [](int i){ std::stringstream str; str << i; return str.str();};
    std::map<int, std::string> m =
        view::zip(view::ints, view::ints | view::transform(to_string)) | view::take(5);
    using P = std::pair<int const, std::string>;
    ::check_equal(m, {P{0,"0"}, P{1,"1"}, P{2,"2"}, P{3,"3"}, P{4,"4"}});

    // Another way to say the same thing, but with a range comprehension:
    m = view::for_each(view::ints(0,5), [&](int i) {
            return yield(std::make_pair(i, to_string(i)));
        });
    ::check_equal(m, {P{0,"0"}, P{1,"1"}, P{2,"2"}, P{3,"3"}, P{4,"4"}});

    CONCEPT_ASSERT(Range<std::set<int>>());
    CONCEPT_ASSERT(!View<std::set<int>>());
    std::set<int> s = view::ints | view::take(10);
    ::check_equal(s, {0,1,2,3,4,5,6,7,8,9});

    static_assert(!ranges::is_view<std::initializer_list<int>>::value, "");

    return ::test_result();
}
