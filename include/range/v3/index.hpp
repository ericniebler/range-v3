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
            CPP_def
            (
                template(typename Rng, typename T)
                class CompatibleDifferenceType,
                    ConvertibleTo<T, range_difference_t<Rng>>
            );

            CPP_def
            (
                template(typename Rng, typename T)
                class Indexable,
                    RandomAccessRange<Rng> &&
                    CompatibleDifferenceType<Rng, T>
            );
        }  // namespace index_detail
        /// \endcond

        /// Unchecked indexed range access.
        ///
        /// \ingroup group-core
        struct index_fn
        {
            /// \return `begin(rng)[n]`
            CPP_template(typename Rng)(
                requires RandomAccessRange<Rng>)
            constexpr /*c++14*/
            range_reference_t<Rng> operator()(Rng &&rng, range_difference_t<Rng> n) const
                noexcept(noexcept(ranges::begin(rng)[n]))
            {
                RANGES_EXPECT(!(bool)SizedRange<Rng> || n < ranges::distance(rng));
                return ranges::begin(rng)[n];
            }
            /// \return `begin(rng)[n]`
            CPP_template(typename Rng, typename T, typename Self = index_fn,
                     typename D = range_difference_t<Rng>)(
                requires RandomAccessRange<Rng> &&
                                  !Same<uncvref_t<T>, D> &&
                                  ConvertibleTo<T, D>)
            constexpr /*c++14*/
            range_reference_t<Rng> operator()(Rng &&rng, T &&t) const
                noexcept(noexcept(ranges::begin(rng)[D()]))
            {
                return Self{}((Rng &&) rng, static_cast<D>((T &&) t));
            }

            // /// \cond
            // CPP_template(typename R, typename T)(
            //     requires not index_detail::Indexable<R, T>)
            // (void) operator()(R &&, T &&) const
            // {
            //     CPP_assert_msg(RandomAccessRange<R>,
            //         "ranges::index(rng, idx): rng argument must be a model of the RandomAccessRange concept.");
            //     CPP_assert_msg(ConvertibleTo<T, range_difference_t<R>>,
            //         "ranges::index(rng, idx): idx argument must be convertible to range_difference_t<rng>.");
            // }
            // /// \endcond
        };

        /// Unchecked indexed range access.
        ///
        /// \ingroup group-core
        /// \sa `index_fn`
        RANGES_INLINE_VARIABLE(index_fn, index)
    }
}

#endif  // RANGES_V3_INDEX_HPP
