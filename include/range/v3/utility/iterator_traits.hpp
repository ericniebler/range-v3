/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2013-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGES_V3_UTILITY_ITERATOR_TRAITS_HPP
#define RANGES_V3_UTILITY_ITERATOR_TRAITS_HPP

#include <type_traits>
#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/utility/iterator_concepts.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-core
        /// @{
        ////////////////////////////////////////////////////////////////////////////////////////////
        // iterator traits
        template<typename I>
        using value_type_t = concepts::Readable::value_t<I>;

        template<typename I>
        using rvalue_reference_t = concepts::Readable::rvalue_reference_t<I>;

        template<typename I>
        using iter_common_reference_t = concepts::Readable::common_reference_t<I>;

        template<typename I>
        using iterator_category_t = concepts::InputIterator::category_t<I>;

        template<typename I>
        using difference_type_t = concepts::WeaklyIncrementable::difference_t<I>;

        template<typename I>
        using size_type_t = meta::_t<std::make_unsigned<difference_type_t<I>>>;
        /// @}

        /// \cond
        template<typename I>
        using iterator_value_t
            RANGES_DEPRECATED("Please use ranges::value_type_t instead") =
                value_type_t<I>;

        template<typename I>
        using iterator_reference_t
            RANGES_DEPRECATED("Please use ranges::reference_t instead") =
                reference_t<I>;

        template<typename I>
        using iterator_rvalue_reference_t
            RANGES_DEPRECATED("Please use ranges::rvalue_reference_t instead") =
                rvalue_reference_t<I>;

        template<typename I>
        using iterator_common_reference_t
            RANGES_DEPRECATED("Please use ranges::iter_common_reference_t instead") =
                iter_common_reference_t<I>;

        template<typename I>
        using iterator_difference_t
            RANGES_DEPRECATED("Please use ranges::difference_type_t instead") =
                difference_type_t<I>;

        template<typename I>
        using iterator_size_t
            RANGES_DEPRECATED("Please use ranges::size_type_t instead") =
                size_type_t<I>;

        template<typename I>
        using iterator_value
            RANGES_DEPRECATED("Please use ranges::value_type instead") =
                value_type<I>;

        template<typename I>
        using iterator_difference
            RANGES_DEPRECATED("Please use ranges::difference_type instead") =
                difference_type<I>;

        template<typename I>
        using iterator_reference
            RANGES_DEPRECATED("iterator_reference is deprecated") =
                meta::defer<reference_t, I>;

        template<typename I>
        using iterator_rvalue_reference
            RANGES_DEPRECATED("iterator_rvalue_reference is deprecated") =
                meta::defer<rvalue_reference_t, I>;

        template<typename I>
        using iterator_common_reference
            RANGES_DEPRECATED("iterator_common_reference is deprecated") =
                meta::defer<iter_common_reference_t, I>;

        template<typename I>
        using iterator_size
            RANGES_DEPRECATED("Please use ranges::size_type instead") =
                size_type<I>;

        namespace detail
        {
            template<typename I>
            using arrow_type_ = decltype(std::declval<I &>().operator->());

            template<typename I>
            struct pointer_type_
              : meta::if_<
                    meta::is_trait<meta::defer<arrow_type_, I>>,
                    meta::defer<arrow_type_, I>,
                    std::add_pointer<reference_t<I>>>
            {};
        }
        /// \endcond
    }
}

#endif // RANGES_V3_UTILITY_ITERATOR_TRAITS_HPP
