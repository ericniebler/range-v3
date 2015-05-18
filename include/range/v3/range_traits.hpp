/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2013-2014
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

#include <utility>
#include <iterator>
#include <type_traits>
#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-core
        /// @{

        // Aliases (SFINAE-able)
        template<typename Rng>
        using range_iterator_t = concepts::View::iterator_t<Rng>;

        template<typename Rng>
        using range_sentinel_t = concepts::View::sentinel_t<Rng>;

        template<typename Rng>
        using range_difference_t = concepts::View::difference_t<Rng>;

        template<typename Rng>
        using range_size_t = meta::eval<std::make_unsigned<range_difference_t<Rng>>>;

        template<typename Rng>
        using range_value_t = concepts::InputView::value_t<Rng>;

        template<typename Rng>
        using range_reference_t = concepts::InputView::reference_t<Rng>;

        template<typename Rng>
        using range_rvalue_reference_t = concepts::InputView::rvalue_reference_t<Rng>;

        template<typename Rng>
        using range_common_reference_t = concepts::InputView::common_reference_t<Rng>;

        template<typename Rng>
        using range_category_t = concepts::InputView::category_t<Rng>;

        template<typename Rng>
        using range_common_iterator_t = common_iterator<range_iterator_t<Rng>, range_sentinel_t<Rng>>;

        template<typename Rng>
        using range_safe_iterator_t = decltype(ranges::safe_begin(std::declval<Rng>()));

        template<typename Rng>
        using range_safe_sentinel_t = decltype(ranges::safe_end(std::declval<Rng>()));

        // Metafunctions
        template<typename Rng>
        using range_iterator = meta::defer<range_iterator_t, Rng>;

        template<typename Rng>
        using range_sentinel = meta::defer<range_sentinel_t, Rng>;

        template<typename Rng>
        using range_category = meta::defer<range_category_t, Rng>;

        template<typename Rng>
        using range_value = meta::defer<range_value_t, Rng>;

        template<typename Rng>
        using range_difference = meta::defer<range_difference_t, Rng>;

        template<typename Rng>
        using range_reference = meta::defer<range_reference_t, Rng>;

        template<typename Rng>
        using range_rvalue_reference = meta::defer<range_rvalue_reference_t, Rng>;

        template<typename Rng>
        using range_common_reference = meta::defer<range_common_reference_t, Rng>;

        template<typename Rng>
        using range_size = meta::defer<range_size_t, Rng>;

        // User customization point for infinite ranges:
        template<typename Rng, typename Void /*= void*/>
        struct is_infinite
          : std::is_base_of<basic_range<true>, Rng>
        {};

        template<typename Rng>
        struct is_infinite<Rng &>
          : is_infinite<Rng>
        {};

        template<typename Rng>
        struct is_infinite<Rng const>
          : is_infinite<Rng>
        {};

        /// @}
    }
}

#endif
