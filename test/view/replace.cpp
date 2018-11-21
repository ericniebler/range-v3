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
    CPP_assert(Same<range_value_t<decltype((rng))>, int>);
    has_type<int const &>(*begin(rng));
    models<ViewConcept>(aux::copy(rng));
    models_not<SizedViewConcept>(aux::copy(rng));
    models_not<BoundedViewConcept>(aux::copy(rng));
    models<InputIteratorConcept>(begin(rng));
    models_not<ForwardIteratorConcept>(begin(rng));

    auto && tmp = rng | view::bounded;
    CPP_assert(Same<range_value_t<decltype((tmp))>, int>);
    has_type<int const &>(*begin(tmp));
    models<BoundedViewConcept>(aux::copy(tmp));
    models_not<SizedViewConcept>(aux::copy(tmp));
    models<InputIteratorConcept>(begin(tmp));
    models_not<ForwardIteratorConcept>(begin(tmp));
    std::vector<int> actual{begin(tmp), end(tmp)};
    ::check_equal(actual, {42, 2, 3, 4, 5, 6, 7, 8, 9, 42, 2, 3, 4, 5, 6, 7, 8, 9, 42, 2, 3, 4, 5, 6, 7, 8, 9});

    std::vector<int> rgi{1,2,3,4,5,6,7,8,9};
    auto &&rng2 = rgi | view::replace(5, 42);
    CPP_assert(Same<range_value_t<decltype((rng2))>, int>);
    has_type<int const &>(*begin(rng2));
    has_type<int const &>(iter_move(begin(rng2)));
    models<ViewConcept>(aux::copy(rng2));
    models<SizedViewConcept>(aux::copy(rng2));
    models<BoundedViewConcept>(aux::copy(rng2));
    models<RandomAccessIteratorConcept>(begin(rng2));
    ::check_equal(rng2, {1,2,3,4,42,6,7,8,9});

    int forty_two = 42;
    auto &&rng3 = rgi | view::replace(5, ref(forty_two));
    CPP_assert(Same<range_value_t<decltype((rng3))>, int>);
    has_type<int &>(*begin(rng3));
    has_type<int const &>(iter_move(begin(rng3)));
    models<ViewConcept>(aux::copy(rng3));
    models<SizedViewConcept>(aux::copy(rng3));
    models<BoundedViewConcept>(aux::copy(rng3));
    models<RandomAccessIteratorConcept>(begin(rng3));
    ::check_equal(rng3, {1,2,3,4,42,6,7,8,9});

    auto &&rng4 = view::ints | view::replace(5,42) | view::take(10);
    CPP_assert(Same<range_value_t<decltype((rng4))>, int>);
    has_type<int>(*begin(rng4));
    has_type<int>(iter_move(begin(rng4)));
    models<ViewConcept>(aux::copy(rng4));
    models<SizedViewConcept>(aux::copy(rng4));
    models_not<BoundedViewConcept>(aux::copy(rng4));
    models<RandomAccessIteratorConcept>(begin(rng4));
    ::check_equal(rng4, {0,1,2,3,4,42,6,7,8,9});

    {
        int const some_ints[] = {1,2,3,4,5,6,7,8,9,1,2,3,4,5,6,7,8,9,1,2,3,4,5,6,7,8,9};
        auto rng = debug_input_view<int const>{some_ints} | view::replace(1, 42);
        ::check_equal(rng, {42,2,3,4,5,6,7,8,9,42,2,3,4,5,6,7,8,9,42,2,3,4,5,6,7,8,9});
    }

    return test_result();
}
