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
#ifndef RANGES_V3_ALGORITHM_MIN_ELEMENT_HPP
#define RANGES_V3_ALGORITHM_MIN_ELEMENT_HPP

#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/iterator.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/invokable.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-algorithms
        /// @{
        struct min_element_fn
        {
            template<typename I, typename S, typename C = ordered_less, typename P = ident,
                typename V = iterator_value_t<I>,
                typename X = concepts::Invokable::result_t<P, V>,
                CONCEPT_REQUIRES_(ForwardIterator<I>() && IteratorRange<I, S>() && Invokable<P, V>() &&
                    InvokableRelation<C, X>())>
            I operator()(I begin, S end, C pred_ = C{}, P proj_ = P{}) const
            {
                auto && pred = invokable(pred_);
                auto && proj = invokable(proj_);
                if(begin != end)
                    for(auto tmp = next(begin); tmp != end; ++tmp)
                        if(pred(proj(*tmp), proj(*begin)))
                            begin = tmp;
                return begin;
            }

            template<typename Rng, typename C = ordered_less, typename P = ident,
                typename I = range_iterator_t<Rng>,
                typename V = iterator_value_t<I>,
                typename X = concepts::Invokable::result_t<P, V>,
                CONCEPT_REQUIRES_(ForwardIterable<Rng &>() && Invokable<P, V>() &&
                    InvokableRelation<C, X>())>
            I operator()(Rng &rng, C pred = C{}, P proj = P{}) const
            {
                return (*this)(begin(rng), end(rng), std::move(pred), std::move(proj));
            }
        };

        /// \sa `min_element_fn`
        /// \ingroup group-algorithms
        constexpr min_element_fn min_element{};

        /// @}
    } // namespace v3
} // namespace ranges

#endif // include guard
