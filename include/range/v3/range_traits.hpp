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
#include <type_traits>
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
        using range_difference_t = concepts::Iterable::difference_t<Rng>;

        template<typename Rng>
        using range_size_t = meta_apply<std::make_unsigned, range_difference_t<Rng>>;

        template<typename Rng>
        using range_value_t = concepts::InputIterable::value_t<Rng>;

        template<typename Rng>
        using range_reference_t = concepts::InputIterable::reference_t<Rng>;

        template<typename Rng>
        using range_category_t = concepts::InputIterable::category_t<Rng>;

        template<typename Rng>
        using range_pointer_t = concepts::InputIterable::pointer_t<Rng>;

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

        // BUGBUG
        //template<typename Rng>
        //struct range_size
        //{
        //    using type = range_size_t<Rng>;
        //};

        // User customization point for infinite ranges:
        template<typename Rng, typename Void /*= void*/>
        struct is_infinite
          : std::is_base_of<detail::is_infinite<true>, Rng>
        {};
    }
}

#endif
