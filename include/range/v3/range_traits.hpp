// Boost.Range library
//
//  Copyright Thorsten Ottosen, Neil Groves 2006 - 2008.
//  Copyright Eric Niebler 2013.
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//

#ifndef RANGES_V3_RANGE_TRAITS_HPP
#define RANGES_V3_RANGE_TRAITS_HPP

#include <utility>
#include <iterator>
#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>

namespace ranges
{
    inline namespace v3
    {
        // Aliases (SFINAE-able)
        template<typename Rng>
        using range_iterator_t = concepts::Iterable::iterator_t<Rng>;

        template<typename Rng>
        using range_sentinel_t = concepts::Iterable::sentinel_t<Rng>;

        template<typename Rng>
        using range_value_t = concepts::Iterable::value_t<Rng>;

        template<typename Rng>
        using range_reference_t = concepts::Iterable::reference_t<Rng>;

        template<typename Rng>
        using range_category_t = concepts::Iterable::category_t<Rng>;

        template<typename Rng>
        using range_difference_t = concepts::Iterable::difference_t<Rng>;

        template<typename Rng>
        using range_pointer_t = concepts::Iterable::pointer_t<Rng>;

        template<typename Rng>
        using range_is_finite_t = concepts::Iterable::is_finite_t<Rng>;

        // Metafunctions
        template<typename Rng>
        struct range_iterator
        {
            using type = range_iterator_t<Rng>;
        };

        template<typename Rng>
        struct range_sentinel
        {
            using type = range_sentinel_t<Rng>;
        };

        template<typename Rng>
        struct range_category
        {
            using type = range_category_t<Rng>;
        };

        template<typename Rng>
        struct range_value
        {
            using type = range_value_t<Rng>;
        };

        template<typename Rng>
        struct range_difference
        {
            using type = range_difference_t<Rng>;
        };

        template<typename Rng>
        struct range_pointer
        {
            using type = range_pointer_t<Rng>;
        };

        template<typename Rng>
        struct range_reference
        {
            using type = range_reference_t<Rng>;
        };

        template<typename Rng>
        struct range_is_finite
        {
            using type = range_is_finite_t<Rng>;
        };
    }
}

#endif
