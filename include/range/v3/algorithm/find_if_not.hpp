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
#ifndef RANGES_V3_ALGORITHM_FIND_IF_NOT_HPP
#define RANGES_V3_ALGORITHM_FIND_IF_NOT_HPP

#include <utility>
#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-algorithms
        /// @{
        struct find_if_not_fn
        {
            /// \brief template function \c find_if_not_fn::operator()
            ///
            /// range-based version of the \c find_if_not std algorithm
            ///
            /// \pre `Rng` is a model of the `Range` concept
            /// \pre `I` is a model of the `InputIterator` concept
            /// \pre `S` is a model of the `Sentinel<I>` concept
            /// \pre `P` is a model of the `Invocable<V>` concept, where `V` is the
            ///      value type of I.
            /// \pre `F` models `Predicate<X>`, where `X` is the result type
            ///      of `Invocable<P, V>`
            template<typename I, typename S, typename F, typename P = ident,
                CONCEPT_REQUIRES_(InputIterator<I>() && Sentinel<S, I>() &&
                    IndirectPredicate<F, projected<I, P> >())>
            I operator()(I begin, S end, F pred, P proj = P{}) const
            {
                for(; begin != end; ++begin)
                    if(!invoke(pred, invoke(proj, *begin)))
                        break;
                return begin;
            }

            /// \overload
            template<typename Rng, typename F, typename P = ident,
                typename I = iterator_t<Rng>,
                CONCEPT_REQUIRES_(InputRange<Rng>() &&
                    IndirectPredicate<F, projected<I, P> >())>
            safe_iterator_t<Rng> operator()(Rng &&rng, F pred, P proj = P{}) const
            {
                return (*this)(begin(rng), end(rng), std::move(pred), std::move(proj));
            }
        };

        /// \sa `find_if_not_fn`
        /// \ingroup group-algorithms
        RANGES_INLINE_VARIABLE(with_braced_init_args<find_if_not_fn>, find_if_not)
        /// @}
    } // namespace v3
} // namespace ranges

#endif // include guard
