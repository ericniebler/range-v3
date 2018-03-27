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

#include <map>
#include <string>
#include <vector>
#include <range/v3/core.hpp>
#include <range/v3/view/iota.hpp>
#include <range/v3/view/map.hpp>
#include <range/v3/view/zip.hpp>
#include <range/v3/utility/copy.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"

int main()
{
    using namespace ranges;
    std::map<std::string, int> m = {
        {"this", 0},
        {"that", 1},
        {"other", 2}};
    auto && keys = m | view::keys;
    has_type<std::string const &>(*begin(keys));
    models<concepts::SizedView>(aux::copy(keys));
    models<concepts::BoundedView>(aux::copy(keys));
    models<concepts::BidirectionalIterator>(begin(keys));
    CHECK(&*begin(keys) == &m.begin()->first);
    ::check_equal(keys, {"other", "that", "this"});

    auto && values = m | view::values;
    has_type<int &>(*begin(values));
    models<concepts::SizedView>(aux::copy(values));
    models<concepts::BoundedView>(aux::copy(values));
    models<concepts::BidirectionalIterator>(begin(values));
    CHECK(&*begin(values) == &m.begin()->second);
    ::check_equal(values, {2, 1, 0});

    {
        // regression test for #526
        auto f = detail::get_first{};
        CONCEPT_ASSERT(Same<int, decltype(f(std::declval<std::pair<int,int>>()))>());
        CONCEPT_ASSERT(Same<int&, decltype(f(std::declval<std::pair<int,int>&>()))>());
        CONCEPT_ASSERT(Same<int&, decltype(f(std::declval<std::pair<int&,int&>>()))>());
        CONCEPT_ASSERT(Same<int&, decltype(f(std::declval<std::pair<int&,int&>&>()))>());
        CONCEPT_ASSERT(Same<int, decltype(f(std::declval<std::pair<int&&,int&&>>()))>());
        CONCEPT_ASSERT(Same<int&, decltype(f(std::declval<std::pair<int&&,int&&>&>()))>());

        std::vector<int> xs = {42, 100, -1234};
        auto exs = view::zip(view::ints, xs);
        ::check_equal(view::keys(exs), {0, 1, 2});
    }

    {
        std::pair<int, int> const data[] = {{0, 2}, {1, 1}, {2, 0}};
        auto key_range = debug_input_view<std::pair<int, int> const>{data} | view::keys;
        check_equal(key_range, {0,1,2});
        auto value_range = debug_input_view<std::pair<int, int> const>{data} | view::values;
        check_equal(value_range, {2,1,0});
    }

    return test_result();
}
