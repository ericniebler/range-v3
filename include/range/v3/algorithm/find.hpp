// Range v3 library
//
//  Copyright Eric Niebler 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
#ifndef RANGES_V3_ALGORITHM_FIND_HPP
#define RANGES_V3_ALGORITHM_FIND_HPP

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
        struct find_fn
        {
            /// \brief template function \c find_fn::operator()
            ///
            /// range-based version of the \c find std algorithm
            ///
            /// \pre \c Rng is a model of the Range concept
            /// \pre \c I is a model of the InputIterator concept
            /// \pre \c S is a model of the Sentinel<I> concept
            /// \pre \c P is a model of the Invokable<iterator_value_t<I>> concept
            /// \pre The ResultType of P is EqualityComparable with V1
            template<typename I, typename S, typename V1, typename P = ident,
                typename V0 = iterator_value_t<I>,
                typename X = concepts::Invokable::result_t<P, V0>,
                CONCEPT_REQUIRES_(
                    InputIterator<I>() && IteratorRange<I, S>() &&
                    Invokable<P, V0>() &&
                    EqualityComparable<X, V1>()
                )>
            I operator()(I begin, S end, V1 const &val, P proj_ = P{}) const
            {
                auto &&proj = invokable(proj_);
                for(; begin != end; ++begin)
                    if(proj(*begin) == val)
                        break;
                return begin;
            }

            /// \overload
            template<typename Rng, typename V1, typename P = ident,
                typename I = range_iterator_t<Rng>,
                typename V0 = iterator_value_t<I>,
                typename X = concepts::Invokable::result_t<P, V0>,
                CONCEPT_REQUIRES_(
                    InputIterable<Rng &>() &&
                    Invokable<P, V0>() &&
                    EqualityComparable<X, V1>()
                )>
            I operator()(Rng &rng, V1 const &val, P proj = P{}) const
            {
                return (*this)(begin(rng), end(rng), val, std::move(proj));
            }
        };

        constexpr with_braced_init_args<find_fn> find {};

    } // namespace v3
} // namespace ranges

#endif // include guard
