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
#ifndef RANGES_V3_ALGORITHM_REMOVE_IF_HPP
#define RANGES_V3_ALGORITHM_REMOVE_IF_HPP

#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/iterator.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/invokable.hpp>
#include <range/v3/algorithm/find_if.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \ingroup group-concepts
        template<typename I, typename C, typename P = ident,
            typename V = iterator_value_t<I>,
            typename X = concepts::Invokable::result_t<P, V>>
        using RemovableIf = meta::fast_and<
            ForwardIterator<I>,
            Invokable<P, V>,
            InvokablePredicate<C, X>,
            Permutable<I>>;

        /// \addtogroup group-algorithms
        /// @{
        struct remove_if_fn
        {
            template<typename I, typename S, typename C, typename P = ident,
                CONCEPT_REQUIRES_(RemovableIf<I, C, P>() && IteratorRange<I, S>())>
            I operator()(I begin, S end, C pred_, P proj_ = P{}) const
            {
                auto &&pred = invokable(pred_);
                auto &&proj = invokable(proj_);
                begin = find_if(std::move(begin), end, std::ref(pred), std::ref(proj));
                if(begin != end)
                {
                    for(I i = next(begin); i != end; ++i)
                    {
                        if(!(pred(proj(*i))))
                        {
                            *begin = std::move(*i);
                            ++begin;
                        }
                    }
                }
                return begin;
            }

            template<typename Rng, typename C, typename P = ident,
                typename I = range_iterator_t<Rng>,
                CONCEPT_REQUIRES_(RemovableIf<I, C, P>() && ForwardIterable<Rng &>())>
            I operator()(Rng &rng, C pred, P proj = P{}) const
            {
                return (*this)(begin(rng), end(rng), std::move(pred), std::move(proj));
            }
        };

        /// \sa `remove_if_fn`
        /// \ingroup group-algorithms
        constexpr remove_if_fn remove_if{};

        /// @}
    } // namespace v3
} // namespace ranges

#endif // include guard
