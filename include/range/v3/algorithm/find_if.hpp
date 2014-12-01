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
#ifndef RANGES_V3_ALGORITHM_FIND_IF_HPP
#define RANGES_V3_ALGORITHM_FIND_IF_HPP

#include <utility>
#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/functional.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-algorithms
        /// @{
        struct find_if_fn
        {
            /// \brief template function \c find_fn::operator()
            ///
            /// range-based version of the \c find std algorithm
            ///
            /// \pre `Rng` is a model of the `Iterable` concept
            /// \pre `I` is a model of the `InputIterator` concept
            /// \pre `S` is a model of the `Sentinel<I>` concept
            /// \pre `P` is a model of the `Invokable<V>` concept, where `V` is the
            ///      value type of I.
            /// \pre `F` models `InvokablePredicate<X>`, where `X` is the result type
            ///      of `Invokable<P, V>`
            template<typename I, typename S, typename F, typename P = ident,
                typename V = iterator_value_t<I>,
                typename X = concepts::Invokable::result_t<P, V>,
                CONCEPT_REQUIRES_(
                    InputIterator<I>() && IteratorRange<I, S>() &&
                    Invokable<P, V>() &&
                    InvokablePredicate<F, X>()
                )>
            I operator()(I begin, S end, F pred_, P proj_ = P{}) const
            {
                auto &&pred = invokable(pred_);
                auto &&proj = invokable(proj_);
                for(; begin != end; ++begin)
                    if(pred(proj(*begin)))
                        break;
                return begin;
            }

            /// \overload
            template<typename Rng, typename F, typename P = ident,
                typename I = range_iterator_t<Rng>,
                typename V = iterator_value_t<I>,
                typename X = concepts::Invokable::result_t<P, V>,
                CONCEPT_REQUIRES_(
                    InputIterable<Rng &>() &&
                    Invokable<P, V>() &&
                    InvokablePredicate<F, X>()
                )>
            I operator()(Rng &rng, F pred, P proj = P{}) const
            {
                return (*this)(begin(rng), end(rng), std::move(pred), std::move(proj));
            }
        };

        /// \sa `find_if_fn`
        /// \ingroup group-algorithms
        constexpr find_if_fn find_if {};

        /// @}
    } // namespace v3
} // namespace ranges

#endif // include guard
