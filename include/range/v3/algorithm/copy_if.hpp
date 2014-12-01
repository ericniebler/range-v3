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
#ifndef RANGES_V3_ALGORITHM_COPY_IF_HPP
#define RANGES_V3_ALGORITHM_COPY_IF_HPP

#include <utility>
#include <functional>
#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/utility/invokable.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/iterator_concepts.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-algorithms
        /// @{
        struct copy_if_fn
        {
            template<typename I, typename S, typename O, typename F, typename P = ident,
                typename V = iterator_value_t<I>,
                typename X = concepts::Invokable::result_t<P, V>,
                CONCEPT_REQUIRES_(
                    InputIterator<I>() && IteratorRange<I, S>() &&
                    WeaklyIncrementable<O>() &&
                    InvokablePredicate<F, X>() &&
                    IndirectlyProjectedCopyable<I, P, O>()
                )>
            std::pair<I, O>
            operator()(I begin, S end, O out, F pred_, P proj_ = P{}) const
            {
                auto &&pred = invokable(pred_);
                auto &&proj = invokable(proj_);
                for(; begin != end; ++begin)
                {
                    if(pred(proj(*begin)))
                    {
                        *out = proj(*begin);
                        ++out;
                    }
                }
                return {begin, out};
            }

            template<typename Rng, typename O, typename F, typename P = ident,
                typename I = range_iterator_t<Rng>,
                typename V = iterator_value_t<I>,
                typename X = concepts::Invokable::result_t<P, V>,
                CONCEPT_REQUIRES_(
                    InputIterable<Rng &>() &&
                    WeaklyIncrementable<O>() &&
                    InvokablePredicate<F, X>() &&
                    IndirectlyProjectedCopyable<I, P, O>()
                )>
            std::pair<I, O>
            operator()(Rng &rng, O out, F pred, P proj = P{}) const
            {
                return (*this)(begin(rng), end(rng), std::move(out), std::move(pred), std::move(proj));
            }
        };

        /// \sa `copy_if_fn`
        /// \ingroup group-algorithms
        constexpr copy_if_fn copy_if{};

        /// @}
    } // namespace v3
} // namespace ranges

#endif // include guard
