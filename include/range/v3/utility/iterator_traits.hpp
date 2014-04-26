// Boost.Range library
//
//  Copyright Eric Niebler 2013.
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//

#ifndef RANGES_V3_UTILITY_ITERATOR_TRAITS_HPP
#define RANGES_V3_UTILITY_ITERATOR_TRAITS_HPP

#include <type_traits>
#include <range/v3/range_fwd.hpp>
#include <range/v3/utility/iterator_concepts.hpp>

namespace ranges
{
    inline namespace v3
    {
        ////////////////////////////////////////////////////////////////////////////////////////////
        // iterator traits
        template<typename It>
        using iterator_value_t = concepts::Readable::value_t<It>;

        template<typename It>
        using iterator_reference_t = concepts::Readable::reference_t<It>;

        template<typename It>
        using iterator_category_t = concepts::WeakInputIterator::category_t<It>;

        template<typename It>
        using iterator_difference_t = concepts::WeaklyIncrementable::difference_t<It>;

        template<typename It>
        using iterator_pointer_t = concepts::Readable::pointer_t<It>;

        template<typename Iterator>
        using iterator_size_t = meta_apply<std::make_unsigned, iterator_difference_t<Iterator>>;

        // Metafunctions
        template<typename It>
        struct iterator_category
        {
            using type = iterator_category_t<It>;
        };

        template<typename It>
        struct iterator_value
        {
            using type = iterator_value_t<It>;
        };

        template<typename It>
        struct iterator_difference
        {
            using type = iterator_difference_t<It>;
        };

        template<typename It>
        struct iterator_pointer
        {
            using type = iterator_pointer_t<It>;
        };

        template<typename It>
        struct iterator_reference
        {
            using type = iterator_reference_t<It>;
        };

        template<typename It>
        struct iterator_size
        {
            using type = iterator_size_t<It>;
        };
    }
}

#endif // RANGES_V3_UTILITY_ITERATOR_TRAITS_HPP
