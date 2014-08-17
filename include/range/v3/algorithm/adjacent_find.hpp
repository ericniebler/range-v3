//  Copyright Eric Niebler 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//
#ifndef RANGES_V3_ALGORITHM_ADJACENT_FIND_HPP
#define RANGES_V3_ALGORITHM_ADJACENT_FIND_HPP

#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/utility/invokable.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/range_algorithm.hpp>

namespace ranges
{
    inline namespace v3
    {
        struct adjacent_find_fn
        {
            /// \brief function template \c adjacent_find_fn::operator()
            ///
            /// range-based version of the \c adjacent_find std algorithm
            ///
            /// \pre \c Rng is a model of the Range concept
            /// \pre \c R is a model of the BinaryPredicate concept
            template<typename I, typename S, typename R = equal_to, typename P = ident,
                typename V = iterator_value_t<I>,
                CONCEPT_REQUIRES_(
                    ForwardIterator<I, S>()                                         &&
                    Invokable<P, V>()                                               &&
                    InvokableRelation<R, concepts::Invokable::result_t<P, V>>()
                )>
            I
            operator()(I begin, S end, R pred = R{}, P proj = P{}) const
            {
                auto &&ipred = invokable(pred);
                auto &&iproj = invokable(proj);
                if(begin == end)
                    return begin;
                auto next = begin;
                for(; ++next != end; begin = next)
                    if(ipred(iproj(*begin), iproj(*next)))
                        return begin;
                return next;
            }

            /// \overload
            template<typename Rng, typename R = equal_to, typename P = ident,
                typename I = range_iterator_t<Rng>,
                typename V = iterator_value_t<I>,
                CONCEPT_REQUIRES_(
                    ForwardRange<Rng>()                                          &&
                    Invokable<P, V>()                                               &&
                    InvokableRelation<R, concepts::Invokable::result_t<P, V>>()
                )>
            I
            operator()(Rng &rng, R pred = R{}, P proj = P{}) const
            {
                return (*this)(begin(rng), end(rng), std::move(pred), std::move(proj));
            }
        };

        RANGES_CONSTEXPR range_algorithm<adjacent_find_fn> adjacent_find {};

    } // namespace v3
} // namespace ranges

#endif // RANGE_ALGORITHM_ADJACENT_FIND_HPP
