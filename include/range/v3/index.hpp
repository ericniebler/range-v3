/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2014-present
//  Copyright Gonzalo Brito Gadeschi 2017
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGES_V3_INDEX_HPP
#define RANGES_V3_INDEX_HPP

#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/distance.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \cond
        namespace index_detail
        {
            template<typename Rng, typename T>
            using Concept = meta::and_<
                RandomAccessRange<Rng>,
                // Only evaluate this one if the previous one succeeded
                meta::lazy::invoke<
                    meta::compose<
                        meta::bind_front<meta::quote<ConvertibleTo>, T>,
                        meta::quote<range_difference_type_t>>,
                    Rng>>;
        }  // namespace index_detail
        /// \endcond

        /// Unchecked indexed range access.
        ///
        /// \ingroup group-core
        struct index_fn
        {
            /// \return `begin(rng)[n]`
            template<typename Rng,
                CONCEPT_REQUIRES_(RandomAccessRange<Rng>())>
            RANGES_CXX14_CONSTEXPR
            auto operator()(Rng &&rng, range_difference_type_t<Rng> n) const
                noexcept(noexcept(ranges::begin(rng)[n]) &&
                         noexcept(n < ranges::distance(rng)))
              ->
                decltype(ranges::begin(rng)[n])
            {
                RANGES_EXPECT(!SizedRange<Rng>() || n < ranges::distance(rng));
                return ranges::begin(rng)[n];
            }
            /// \return `begin(rng)[n]`
            template<typename Rng, typename T, typename Self = index_fn,
                     typename D = range_difference_type_t<Rng>,
                CONCEPT_REQUIRES_(RandomAccessRange<Rng>() &&
                                  !Same<uncvref_t<T>, D>() &&
                                  ConvertibleTo<T, D>())>
            RANGES_CXX14_CONSTEXPR
            auto operator()(Rng &&rng, T &&t) const
            RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
            (
                Self{}((Rng &&) rng, static_cast<D>((T &&) t))
            )

            /// \cond
            template<typename R, typename T,
                CONCEPT_REQUIRES_(!index_detail::Concept<R, T>())>
            void operator()(R&&, T&&) const
            {
                CONCEPT_ASSERT_MSG(RandomAccessRange<R>(),
                    "ranges::index(rng, idx): rng argument must be a model of the RandomAccessRange concept.");
                CONCEPT_ASSERT_MSG(ConvertibleTo<T, range_difference_type_t<R>>(),
                    "ranges::index(rng, idx): idx argument must be convertible to range_difference_type_t<rng>.");
            }
            /// \endcond
        };

        /// Unchecked indexed range access.
        ///
        /// \ingroup group-core
        /// \sa `index_fn`
        RANGES_INLINE_VARIABLE(index_fn, index)
    }
}

#endif  // RANGES_V3_INDEX_HPP
