//  Copyright Eric Niebler 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//
#ifndef RANGES_V3_ALGORITHM_FOR_EACH_HPP
#define RANGES_V3_ALGORITHM_FOR_EACH_HPP

#include <functional>
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
        struct for_each_fn
        {
            template<typename I, typename S, typename F, typename P = ident,
                typename V = iterator_value_t<I>,
                CONCEPT_REQUIRES_(
                    InputIterator<I, S>()                               &&
                    Invokable<P, V>()                                   &&
                    Invokable<F, concepts::Invokable::result_t<P, V>>()
                )>
            I
            operator()(I begin, S end, F fun, P proj = P{}) const
            {
                auto &&ifun = invokable(fun);
                auto &&iproj = invokable(proj);
                for(; begin != end; ++begin)
                {
                    ifun(iproj(*begin));
                }
                return begin;
            }

            template<typename Rng, typename F, typename P = ident,
                typename I = range_iterator_t<Rng>,
                typename V = iterator_value_t<I>,
                CONCEPT_REQUIRES_(
                    InputIterable<Rng>()                                &&
                    Invokable<P, V>()                                   &&
                    Invokable<F, concepts::Invokable::result_t<P, V>>()
                )>
            I
            operator()(Rng &rng, F fun, P proj = P{}) const
            {
                return (*this)(begin(rng), end(rng), std::move(fun), std::move(proj));
            }
        };

        RANGES_CONSTEXPR range_algorithm<for_each_fn> for_each{};
    } // namespace v3
} // namespace ranges

#endif // include guard
