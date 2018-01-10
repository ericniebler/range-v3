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

#include <string>
#include <sstream>
#include <vector>
#include <range/v3/core.hpp>
#include <range/v3/istream_range.hpp>
#include <range/v3/view/iota.hpp>
#include <range/v3/view/replace.hpp>
#include <range/v3/view/bounded.hpp>
#include <range/v3/view/take.hpp>
#include <range/v3/utility/copy.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"

int main()
{
    using namespace ranges;

    std::string str{"1 2 3 4 5 6 7 8 9 1 2 3 4 5 6 7 8 9 1 2 3 4 5 6 7 8 9 "};
    std::stringstream sin{str};

    auto && rng = istream<int>(sin) | view::replace(1, 42);
    CONCEPT_ASSERT(Same<range_value_type_t<decltype((rng))>, int>());
    has_type<int const &>(*begin(rng));
    models<concepts::View>(aux::copy(rng));
    models_not<concepts::SizedView>(aux::copy(rng));
    models_not<concepts::BoundedView>(aux::copy(rng));
    models<concepts::InputIterator>(begin(rng));
    models_not<concepts::ForwardIterator>(begin(rng));

    auto && tmp = rng | view::bounded;
    CONCEPT_ASSERT(Same<range_value_type_t<decltype((tmp))>, int>());
    has_type<int const &>(*begin(tmp));
    models<concepts::BoundedView>(aux::copy(tmp));
    models_not<concepts::SizedView>(aux::copy(tmp));
    models<concepts::InputIterator>(begin(tmp));
    models_not<concepts::ForwardIterator>(begin(tmp));
    std::vector<int> actual{begin(tmp), end(tmp)};
    ::check_equal(actual, {42, 2, 3, 4, 5, 6, 7, 8, 9, 42, 2, 3, 4, 5, 6, 7, 8, 9, 42, 2, 3, 4, 5, 6, 7, 8, 9});

    std::vector<int> rgi{1,2,3,4,5,6,7,8,9};
    auto &&rng2 = rgi | view::replace(5, 42);
    CONCEPT_ASSERT(Same<range_value_type_t<decltype((rng2))>, int>());
    has_type<int const &>(*begin(rng2));
    has_type<int const &>(iter_move(begin(rng2)));
    models<concepts::View>(aux::copy(rng2));
    models<concepts::SizedView>(aux::copy(rng2));
    models<concepts::BoundedView>(aux::copy(rng2));
    models<concepts::RandomAccessIterator>(begin(rng2));
    ::check_equal(rng2, {1,2,3,4,42,6,7,8,9});

    int forty_two = 42;
    auto &&rng3 = rgi | view::replace(5, ref(forty_two));
    CONCEPT_ASSERT(Same<range_value_type_t<decltype((rng3))>, int>());
    has_type<int &>(*begin(rng3));
    has_type<int const &>(iter_move(begin(rng3)));
    models<concepts::View>(aux::copy(rng3));
    models<concepts::SizedView>(aux::copy(rng3));
    models<concepts::BoundedView>(aux::copy(rng3));
    models<concepts::RandomAccessIterator>(begin(rng3));
    ::check_equal(rng3, {1,2,3,4,42,6,7,8,9});

    auto &&rng4 = view::ints | view::replace(5,42) | view::take(10);
    CONCEPT_ASSERT(Same<range_value_type_t<decltype((rng4))>, int>());
    has_type<int>(*begin(rng4));
    has_type<int>(iter_move(begin(rng4)));
    models<concepts::View>(aux::copy(rng4));
    models<concepts::SizedView>(aux::copy(rng4));
    models_not<concepts::BoundedView>(aux::copy(rng4));
    models<concepts::RandomAccessIterator>(begin(rng4));
    ::check_equal(rng4, {0,1,2,3,4,42,6,7,8,9});

    {
        int const some_ints[] = {1,2,3,4,5,6,7,8,9,1,2,3,4,5,6,7,8,9,1,2,3,4,5,6,7,8,9};
        auto rng = debug_input_view<int const>{some_ints} | view::replace(1, 42);
        ::check_equal(rng, {42,2,3,4,5,6,7,8,9,42,2,3,4,5,6,7,8,9,42,2,3,4,5,6,7,8,9});
    }

    return test_result();
}
