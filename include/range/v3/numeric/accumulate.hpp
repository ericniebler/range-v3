///////////////////////////////////////////////////////////////////////////////
/// \file accumulate.hpp
///   Contains range-based versions of the std generic numeric operation:
///     accumulate.
//
// Copyright 2004 Eric Niebler.
// Copyright 2006 Thorsten Ottosen.
// Copyright 2009 Neil Groves.
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef RANGES_V3_NUMERIC_ACCUMULATE_HPP
#define RANGES_V3_NUMERIC_ACCUMULATE_HPP

#include <range/v3/begin_end.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/utility/invokable.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/range_algorithm.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/iterator_concepts.hpp>

namespace ranges
{
    inline namespace v3
    {
        template<typename I, typename T, typename Op = plus, typename P = ident,
            typename V = iterator_value_t<I>,
            typename X = concepts::Invokable::result_t<P, V>,
            typename Y = concepts::Invokable::result_t<Op, T, X>>
        using Accumulateable = logical_and_t<
            InputIterator<I>,
            Invokable<P, V>,
            Invokable<Op, T, X>,
            Assignable<T &, Y>>;

        struct accumulate_fn
        {
            template<typename I, typename S, typename T, typename Op = plus, typename P = ident,
                CONCEPT_REQUIRES_(IteratorRange<I, S>() && Accumulateable<I, T, Op, P>())>
            T operator()(I begin, S end, T init, Op op_ = Op{}, P proj_ = P{}) const
            {
                auto &&op = invokable(op_);
                auto &&proj = invokable(proj_);
                for(; begin != end; ++begin)
                    init = op(init, proj(*begin));
                return init;
            }

            template<typename Rng, typename T, typename Op = plus, typename P = ident,
                typename I = range_iterator_t<Rng>,
                CONCEPT_REQUIRES_(Iterable<Rng>() && Accumulateable<I, T, Op, P>())>
            T operator()(Rng && rng, T init, Op op = Op{}, P proj = P{}) const
            {
                return (*this)(begin(rng), end(rng), std::move(init), std::move(op),
                    std::move(proj));
            }
        };

        RANGES_CONSTEXPR range_algorithm<accumulate_fn> accumulate{};
    }
}

#endif
