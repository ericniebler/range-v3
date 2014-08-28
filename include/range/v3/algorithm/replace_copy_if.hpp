//  Copyright Eric Niebler 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//
#ifndef RANGES_V3_ALGORITHM_REPLACE_COPY_IF_HPP
#define RANGES_V3_ALGORITHM_REPLACE_COPY_IF_HPP

#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/invokable.hpp>
#include <range/v3/utility/range_algorithm.hpp>

namespace ranges
{
    inline namespace v3
    {
        template<typename I, typename O, typename C, typename T, typename P = ident,
            typename V = iterator_value_t<I>,
            typename X = concepts::Invokable::result_t<P, V>>
        constexpr bool ReplaceCopyIfable()
        {
            return InputIterator<I>() &&
                   WeakOutputIterator<O, T>() &&
                   IndirectlyCopyable<I, O>() &&
                   Invokable<P, V>() &&
                   InvokablePredicate<C, X>();
        }

        struct replace_copy_if_fn
        {
            template<typename I, typename S, typename O, typename C, typename T, typename P = ident,
                CONCEPT_REQUIRES_(ReplaceCopyIfable<I, O, C, T, P>() && Sentinel<S, I>())>
            std::pair<I, O> operator()(I begin, S end, O out, C pred_, T const & new_value, P proj_ = {}) const
            {
                auto &&pred = invokable(pred_);
                auto &&proj = invokable(proj_);
                for(; begin != end; ++begin, ++out)
                    if(pred(proj(*begin)))
                        *out = new_value;
                    else
                        *out = *begin;
                return {begin, out};
            }

            template<typename Rng, typename O, typename C, typename T, typename P = ident,
                typename I = range_iterator_t<Rng>,
                CONCEPT_REQUIRES_(ReplaceCopyIfable<I, O, C, T, P>() && Iterable<Rng>())>
            std::pair<I, O> operator()(Rng & rng, O out, C pred, T const & new_value, P proj = {}) const
            {
                return (*this)(begin(rng), end(rng), std::move(out), std::move(pred), new_value, std::move(proj));
            }
        };

        RANGES_CONSTEXPR replace_copy_if_fn replace_copy_if{};

    } // namespace v3
} // namespace ranges

#endif // include guard
