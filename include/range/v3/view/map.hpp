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

#ifndef RANGES_V3_VIEW_MAP_HPP
#define RANGES_V3_VIEW_MAP_HPP

#include <utility>
#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/view/view.hpp>
#include <range/v3/view/transform.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \cond
        namespace detail
        {
            struct get_first
            {
                template<typename Pair>
                auto operator()(Pair && p) const ->
                    decltype((std::forward<Pair>(p).first))
                {
                    return std::forward<Pair>(p).first;
                }
            };

            struct get_second
            {
                template<typename Pair>
                auto operator()(Pair && p) const ->
                    decltype((std::forward<Pair>(p).second))
                {
                    return std::forward<Pair>(p).second;
                }
            };

            template<typename T>
            using PairLike = meta::and_<Function<get_first, T>, Function<get_second, T>>;
        }
        /// \endcond

        /// \addtogroup group-views
        /// @{
        namespace view
        {
            struct keys_fn
            {
                template<typename Rng>
                using Concept = meta::and_<
                    InputRange<Rng>,
                    detail::PairLike<range_value_t<Rng>>>;

                template<typename Rng,
                    CONCEPT_REQUIRES_(Concept<Rng>())>
                keys_range_view<all_t<Rng>> operator()(Rng && rng) const
                {
                    return {all(std::forward<Rng>(rng)), detail::get_first{}};
                }
            #ifndef RANGES_DOXYGEN_INVOKED
                template<typename Rng,
                    CONCEPT_REQUIRES_(!Concept<Rng>())>
                void operator()(Rng &&) const
                {
                    CONCEPT_ASSERT_MSG(InputRange<Rng>(),
                        "The argument of view::keys must be a model of the InputRange concept.");
                    CONCEPT_ASSERT_MSG(detail::PairLike<range_value_t<Rng>>(),
                        "The value type of the range passed to view::keys must look like a std::pair; "
                        "That is, it must have first and second data members.");
                }
            #endif
            };

            struct values_fn
            {
                template<typename Rng>
                using Concept = meta::and_<
                    InputRange<Rng>,
                    detail::PairLike<range_value_t<Rng>>>;

                template<typename Rng,
                    CONCEPT_REQUIRES_(Concept<Rng>())>
                values_view<all_t<Rng>> operator()(Rng && rng) const
                {
                    return {all(std::forward<Rng>(rng)), detail::get_second{}};
                }
            #ifndef RANGES_DOXYGEN_INVOKED
                template<typename Rng,
                    CONCEPT_REQUIRES_(!Concept<Rng>())>
                void operator()(Rng &&) const
                {
                    CONCEPT_ASSERT_MSG(InputRange<Rng>(),
                        "The argument of view::values must be a model of the InputRange concept.");
                    CONCEPT_ASSERT_MSG(detail::PairLike<range_value_t<Rng>>(),
                        "The value type of the range passed to view::values must look like a std::pair; "
                        "That is, it must have first and second data members.");
                }
            #endif
            };

            /// \relates keys_fn
            /// \ingroup group-views
            RANGES_INLINE_VARIABLE(view<keys_fn>, keys)

            /// \relates values_fn
            /// \ingroup group-views
            RANGES_INLINE_VARIABLE(view<values_fn>, values)
        }
        /// @}
    }
}

#endif
