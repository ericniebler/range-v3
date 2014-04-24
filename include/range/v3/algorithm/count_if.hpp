//  Copyright Eric Niebler 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//
#ifndef RANGES_V3_ALGORITHM_COUNT_IF_HPP
#define RANGES_V3_ALGORITHM_COUNT_IF_HPP

#include <utility>
#include <initializer_list>
#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/invokable.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>

namespace ranges
{
    inline namespace v3
    {
        struct count_if_fn
        {
            template<typename I, typename S, typename R, typename P = ident,
                typename V0 = iterator_value_t<I>,
                typename X = concepts::Invokable::result_t<P, V0>,
                CONCEPT_REQUIRES_(
                    InputIterator<I, S>()       &&
                    Invokable<P, V0>()          &&
                    InvokablePredicate<R, X>()
                )>
            iterator_difference_t<I>
            operator()(I begin, S end, R pred_, P proj_ = P{}) const
            {
                auto &&pred = invokable(pred_);
                auto &&proj = invokable(proj_);
                iterator_difference_t<I> n = 0;
                for(; begin != end; ++begin)
                    if(pred(proj(*begin)))
                        ++n;
                return n;
            }

            template<typename Rng, typename R, typename P = ident,
                typename I = range_iterator_t<Rng>,
                typename V0 = iterator_value_t<I>,
                typename X = concepts::Invokable::result_t<P, V0>,
                CONCEPT_REQUIRES_(
                    Iterable<Rng>()             &&
                    InputIterator<I>()          &&
                    Invokable<P, V0>()          &&
                    InvokablePredicate<R, X>()
                )>
            iterator_difference_t<I>
            operator()(Rng &&rng, R pred, P proj = P{}) const
            {
                return (*this)(begin(rng), end(rng), std::move(pred), std::move(proj));
            }

            template<typename V0, typename R, typename P = ident,
                typename I = V0 const *,
                typename X = concepts::Invokable::result_t<P, V0>,
                CONCEPT_REQUIRES_(
                    Invokable<P, V0>()          &&
                    InvokablePredicate<R, X>()
                )>
            iterator_difference_t<I>
            operator()(std::initializer_list<V0> rng, R pred, P proj = P{}) const
            {
                return (*this)(rng.begin(), rng.end(), std::move(pred), std::move(proj));
            }
        };

        RANGES_CONSTEXPR count_if_fn count_if{};
    } // namespace v3
} // namespace ranges

#endif // include guard
