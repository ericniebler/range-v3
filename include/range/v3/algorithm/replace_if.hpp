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
#ifndef RANGES_V3_ALGORITHM_REPLACE_IF_HPP
#define RANGES_V3_ALGORITHM_REPLACE_IF_HPP

#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/invokable.hpp>

namespace ranges
{
    inline namespace v3
    {
        template<typename I, typename C, typename T, typename P = ident,
            typename V = iterator_value_t<I>,
            typename X = concepts::Invokable::result_t<P, V>>
        using ReplaceIfable = meta::fast_and<
            InputIterator<I>,
            Invokable<P, V>,
            InvokablePredicate<C, X>,
            Writable<I, T>>;

        struct replace_if_fn
        {
            template<typename I, typename S, typename C, typename T, typename P = ident,
                CONCEPT_REQUIRES_(ReplaceIfable<I, C, T, P>() && IteratorRange<I, S>())>
            I operator()(I begin, S end, C pred_, T const & new_value, P proj_ = P{}) const
            {
                auto &&pred = invokable(pred_);
                auto &&proj = invokable(proj_);
                for(; begin != end; ++begin)
                    if(pred(proj(*begin)))
                        *begin = new_value;
                return begin;
            }

            template<typename Rng, typename C, typename T, typename P = ident,
                typename I = range_iterator_t<Rng>,
                CONCEPT_REQUIRES_(ReplaceIfable<I, C, T, P>() && Iterable<Rng &>())>
            I operator()(Rng & rng, C pred, T const & new_value, P proj = P{}) const
            {
                return (*this)(begin(rng), end(rng), std::move(pred), new_value, std::move(proj));
            }
        };

        constexpr replace_if_fn replace_if{};

    } // namespace v3
} // namespace ranges

#endif // include guard
