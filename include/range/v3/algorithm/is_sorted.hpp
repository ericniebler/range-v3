/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2013-present
//  Copyright Gonzalo Brito Gadeschi 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
// Implementation based on the code in libc++
//   http://http://libcxx.llvm.org/
#ifndef RANGES_V3_ALGORITHM_IS_SORTED_HPP
#define RANGES_V3_ALGORITHM_IS_SORTED_HPP

#include <range/v3/algorithm/is_sorted_until.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-algorithms
        /// @{
        struct is_sorted_fn
        {
            /// \brief template function \c is_sorted_fn::operator()
            ///
            /// range-based version of the \c is_sorted std algorithm
            ///
            /// Works on ForwardRanges
            ///
            /// \pre `Rng` is a model of the `ForwardRange` concept
            /// \pre `I` is a model of the `ForwardIterator` concept
            /// \pre `S` and `I` model the `Sentinel<S, I>` concept
            /// \pre `R` and `projected<I, P>` model the `IndirectRelation<R, projected<I, P>>` concept
            ///
            template<typename I, typename S, typename R = ordered_less, typename P = ident,
                CONCEPT_REQUIRES_(ForwardIterator<I>() && Sentinel<S, I>() &&
                       IndirectRelation<R, projected<I, P>>())>
            bool operator()(I begin, S end, R rel = R{}, P proj = P{}) const
            {
                return is_sorted_until(std::move(begin), end, std::move(rel),
                                       std::move(proj)) == end;
            }

            template<typename Rng, typename R = ordered_less, typename P = ident,
                typename I = iterator_t<Rng>,
                CONCEPT_REQUIRES_(ForwardRange<Rng>() &&
                    IndirectRelation<R, projected<I, P>>())>
            bool operator()(Rng &&rng, R rel = R{}, P proj = P{}) const
            {
                return (*this)(begin(rng), end(rng), std::move(rel), std::move(proj));
            }
        };

        /// \sa `is_sorted_fn`
        /// \ingroup group-algorithms
        RANGES_INLINE_VARIABLE(with_braced_init_args<is_sorted_fn>, is_sorted)
        /// @}
    } // namespace v3
} // namespace ranges

#endif // include guard
