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

#ifndef RANGES_V3_AT_HPP
#define RANGES_V3_AT_HPP

#include <stdexcept>
#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/distance.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/index.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// Checked indexed range access.
        ///
        /// \ingroup group-core
        struct at_fn
        {
            /// \return `begin(rng)[n]`
            CONCEPT_template(typename Rng)(
                requires RandomAccessRange<Rng> && SizedRange<Rng>)
            (RANGES_CXX14_CONSTEXPR
            range_reference_t<Rng>) operator()(Rng &&rng, range_difference_type_t<Rng> n) const
            {
                // Workaround https://gcc.gnu.org/bugzilla/show_bug.cgi?id=67371 in GCC 5
                check_throw(rng, n);
                return ranges::begin(rng)[n];
            }
            /// \return `begin(rng)[n]`
            CONCEPT_template(typename Rng)(
                requires RandomAccessRange<Rng> && !SizedRange<Rng>)
            // (attribute(RANGES_DEPRECATED(
            //     "Checked indexed range access (ranges::at) on !SizedRanges is deprecated! "
            //     "This version performs unchecked access (the range size cannot be computed in O(1) for !SizedRanges)! "
            //     "Use ranges::index for unchecked access instead!"))
            (RANGES_DEPRECATED(
                "Checked indexed range access (ranges::at) on !SizedRanges is deprecated! "
                "This version performs unchecked access (the range size cannot be computed in O(1) for !SizedRanges)! "
                "Use ranges::index for unchecked access instead!")
            RANGES_CXX14_CONSTEXPR
            range_reference_t<Rng>) operator()(Rng &&rng, range_difference_type_t<Rng> n) const
            RANGES_AUTO_RETURN_NOEXCEPT
            (
                index(std::forward<Rng>(rng), n)
            )

            /// \return `begin(rng)[n]`
            CONCEPT_template(typename Rng, typename T, typename Self = at_fn,
                     typename D = range_difference_type_t<Rng>)(
                requires RandomAccessRange<Rng> &&
                                  !Same<uncvref_t<T>, D> &&
                                  ConvertibleTo<T, D>)
            (RANGES_CXX14_CONSTEXPR
            range_reference_t<Rng>) operator()(Rng &&rng, T &&t) const
            RANGES_AUTO_RETURN_NOEXCEPT
            (
                Self{}((Rng &&) rng, static_cast<D>((T &&) t))
            )

            /// \cond
            CONCEPT_template(typename R, typename T)(
                requires not index_detail::Indexable<R, T>)
            (void) operator()(R &&, T &&) const
            {
                CONCEPT_assert_msg(RandomAccessRange<R>,
                    "ranges::at(rng, idx): rng argument must be a model of the RandomAccessRange concept.");
                CONCEPT_assert_msg(ConvertibleTo<T, range_difference_type_t<R>>,
                    "ranges::at(rng, idx): idx argument must be convertible to range_difference_type_t<rng>.");
            }

        private:
            template<class Rng>
            RANGES_CXX14_CONSTEXPR
            static void check_throw(Rng &&rng, range_difference_type_t<Rng> n)
            {
                (n < 0 || n >= ranges::distance(rng)) ? throw std::out_of_range("ranges::at") : void(0);
            }
            /// \endcond
        };

        /// Checked indexed range access.
        ///
        /// \ingroup group-core
        /// \sa `at_fn`
        RANGES_INLINE_VARIABLE(at_fn, at)
    }
}

#endif
