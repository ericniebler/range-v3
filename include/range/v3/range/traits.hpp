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

#ifndef RANGES_V3_RANGE_TRAITS_HPP
#define RANGES_V3_RANGE_TRAITS_HPP

#include <array>
#include <utility>
#include <iterator>
#include <type_traits>
#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range/access.hpp>
#include <range/v3/range/primitives.hpp>

namespace ranges
{
    /// \cond
    namespace detail
    {
        template<typename I, typename S>
        using common_iterator_t =
            meta::if_c<(bool)(Iterator<I> && Sentinel<S, I>), common_iterator<I, S>>;
    }
    /// \endcond

    /// \addtogroup group-core
    /// @{
    template<typename I, typename S>
    using common_iterator_t =
        meta::if_<std::is_same<I, S>, I, detail::common_iterator_t<I, S>>;

    // Aliases (SFINAE-able)
    template<typename Rng>
    using iterator_t = decltype(begin(std::declval<Rng &>()));

    template<typename Rng>
    using sentinel_t = decltype(end(std::declval<Rng &>()));

    template<typename Rng>
    using range_difference_t = iter_difference_t<iterator_t<Rng>>;

    template<typename Rng>
    using range_value_t = iter_value_t<iterator_t<Rng>>;

    template<typename Rng>
    using range_reference_t = iter_reference_t<iterator_t<Rng>>;

    template<typename Rng>
    using range_rvalue_reference_t = iter_rvalue_reference_t<iterator_t<Rng>>;

    template<typename Rng>
    using range_common_reference_t = iter_common_reference_t<iterator_t<Rng>>;

    template<typename Rng>
    using range_size_t = decltype(ranges::size(std::declval<Rng &>()));

    /// \cond
    template<typename Rng>
    using range_difference_type_t
        RANGES_DEPRECATED("range_difference_type_t is deprecated. Use the range_difference_t instead.") =
            iter_difference_t<iterator_t<Rng>>;

    template<typename Rng>
    using range_value_type_t
        RANGES_DEPRECATED("range_value_type_t is deprecated. Use the range_value_t instead.") =
            iter_value_t<iterator_t<Rng>>;

    template<typename Rng>
    using range_category_t
        RANGES_DEPRECATED("range_category_t is deprecated. Use the range concepts instead.") =
            iterator_category_t<iterator_t<Rng>>;

    template<typename Rng>
    using range_size_type_t
        RANGES_DEPRECATED("range_size_type_t is deprecated. Use range_size_t instead.") =
            meta::_t<std::make_unsigned<range_difference_t<Rng>>>;
    /// \endcond

    template<typename Rng>
    using range_common_iterator_t = common_iterator_t<iterator_t<Rng>, sentinel_t<Rng>>;

    /// \cond
    namespace detail
    {
        std::integral_constant<cardinality, finite> test_cardinality(void *);
        template<cardinality Card>
        std::integral_constant<cardinality, Card> test_cardinality(basic_view<Card> *);
        template<typename T, std::size_t N>
        std::integral_constant<cardinality, static_cast<cardinality>(N)>
        test_cardinality(T(*)[N]);
        template<typename T, std::size_t N>
        std::integral_constant<cardinality, static_cast<cardinality>(N)>
        test_cardinality(std::array<T, N>*);
    }
    /// \endcond

    // User customization point for specifying the cardinality of ranges:
    template<typename Rng, typename Void /*= void*/>
    struct range_cardinality
      : meta::if_<std::is_same<Rng, uncvref_t<Rng>>,
            decltype(detail::test_cardinality(static_cast<uncvref_t<Rng> *>(nullptr))),
            range_cardinality<uncvref_t<Rng>>>
    {};

    /// @}
}

#endif
