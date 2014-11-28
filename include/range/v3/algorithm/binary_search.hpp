// Range v3 library
//
//  Copyright Eric Niebler 2013-2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
#ifndef RANGES_V3_ALGORITHM_BINARY_SEARCH_HPP
#define RANGES_V3_ALGORITHM_BINARY_SEARCH_HPP

#include <utility>
#include <functional>
#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/invokable.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/algorithm/lower_bound.hpp>

namespace ranges
{
    inline namespace v3
    {
        struct binary_search_fn
        {
            /// \brief function template \c binary_search_fn::operator()
            ///
            /// range-based version of the \c binary_search std algorithm
            ///
            /// \pre \c Rng is a model of the Rng concept
            /// \pre \c C is a model of the InvokableRelation concept
            template<typename I, typename S, typename V2, typename C = ordered_less, typename P = ident,
                CONCEPT_REQUIRES_(
                    IteratorRange<I, S>() &&
                    BinarySearchable<I, V2, C, P>()
                )>
            bool
            operator()(I begin, S end, V2 const &val, C pred = C{}, P proj = P{}) const
            {
                begin = lower_bound(std::move(begin), end, val, pred, proj);
                auto &&ipred = invokable(pred);
                auto &&iproj = invokable(proj);
                return begin != end && !ipred(val, iproj(*begin));
            }

            /// \overload
            template<typename Rng, typename V2, typename C = ordered_less, typename P = ident,
                typename I = range_iterator_t<Rng>,
                CONCEPT_REQUIRES_(
                    Iterable<Rng>() &&
                    BinarySearchable<I, V2, C, P>()
                )>
            bool
            operator()(Rng &&rng, V2 const &val, C pred = C{}, P proj = P{}) const
            {
                static_assert(!is_infinite<Rng>::value,
                    "Trying to binary search an infinite range");
                return (*this)(begin(rng), end(rng), val, std::move(pred), std::move(proj));
            }
        };

        constexpr with_braced_init_args<binary_search_fn> binary_search{};

    } // namespace v3
} // namespace ranges

#endif // include guard
