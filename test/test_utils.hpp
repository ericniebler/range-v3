//  Copyright Eric Niebler 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef RANGES_TEST_UTILS_HPP
#define RANGES_TEST_UTILS_HPP

#include <algorithm>
#include <initializer_list>
#include <range/v3/distance.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>

template<typename Rng, typename Val>
void check_equal(Rng && actual, std::initializer_list<Val> expected)
{
    auto begin0 = ranges::begin(actual);
    auto end0 = ranges::end(actual);
    auto begin1 = ranges::begin(expected), end1 = ranges::end(expected);
    for(; begin0 != end0 && begin1 != end1; ++begin0, ++begin1)
        CHECK(*begin0 == *begin1);
    CHECK(begin0 == end0);
    CHECK(begin1 == end1);
}

template<typename Expected, typename Actual>
void has_type(Actual &&)
{
    static_assert(std::is_same<Expected, Actual>::value, "Not the same");
}

template<typename Concept, typename Type>
void models(Type &&)
{
    CONCEPT_ASSERT(ranges::concepts::models<Concept, Type>());
}

template<typename Concept, typename Type>
void models_not(Type &&)
{
    CONCEPT_ASSERT(!ranges::concepts::models<Concept, Type>());
}

template<typename T>
T & as_lvalue(T && t)
{
    return t;
}

#endif
