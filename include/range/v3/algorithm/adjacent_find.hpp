/// \file
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
#ifndef RANGES_V3_ALGORITHM_ADJACENT_FIND_HPP
#define RANGES_V3_ALGORITHM_ADJACENT_FIND_HPP

#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-algorithms
        /// @{
        struct adjacent_find_fn
        {
            /// \brief function template \c adjacent_find_fn::operator()
            ///
            /// range-based version of the \c adjacent_find std algorithm
            ///
            /// \pre `Rng` is a model of the `Iterable` concept
            /// \pre `C` is a model of the `BinaryPredicate` concept
            template<typename I, typename S, typename C = equal_to, typename P = ident,
                CONCEPT_REQUIRES_(ForwardIterator<I>() && IteratorRange<I, S>() &&
                    IndirectCallableRelation<C, Project<I, P>>())>
            I
            RANGES_CXX14_CONSTEXPR
            operator()(I begin, S end, C pred_ = C{}, P proj_ = P{}) const
            {
                auto &&pred = as_function(pred_);
                auto &&proj = as_function(proj_);
                if(begin == end)
                    return begin;
                auto next = begin;
                for(; ++next != end; begin = next)
                    if(pred(proj(*begin), proj(*next)))
                        return begin;
                return next;
            }

            /// \overload
            template<typename Rng, typename C = equal_to, typename P = ident,
                typename I = range_iterator_t<Rng>,
                CONCEPT_REQUIRES_(ForwardIterable<Rng>() &&
                    IndirectCallableRelation<C, Project<I, P>>())>
            RANGES_CXX14_CONSTEXPR
            range_safe_iterator_t<Rng>
            operator()(Rng &&rng, C pred = C{}, P proj = P{}) const
            {
                return (*this)(begin(rng), end(rng), std::move(pred), std::move(proj));
            }
        };

        /// \sa `adjacent_find_fn`
        /// \ingroup group-algorithms
        namespace
        {
            constexpr auto&& adjacent_find = static_const<with_braced_init_args<adjacent_find_fn>>::value;
        }

        /// @}

    } // namespace v3
} // namespace ranges

#endif // RANGE_ALGORITHM_ADJACENT_FIND_HPP
