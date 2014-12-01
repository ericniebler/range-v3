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
#ifndef RANGES_V3_ALGORITHM_UPPER_BOUND_HPP
#define RANGES_V3_ALGORITHM_UPPER_BOUND_HPP

#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/distance.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/algorithm/aux_/upper_bound_n.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-algorithms
        /// @{
        struct upper_bound_fn
        {
            template<typename I, typename S, typename V2, typename C = ordered_less, typename P = ident,
                CONCEPT_REQUIRES_(IteratorRange<I, S>() && BinarySearchable<I, V2, C, P>())>
            I operator()(I begin, S end, V2 const &val, C pred = C{}, P proj = P{}) const
            {
                return aux::upper_bound_n(std::move(begin), distance(begin, end), val, std::move(pred),
                    std::move(proj));
            }

            /// \overload
            template<typename Rng, typename V2, typename C = ordered_less, typename P = ident,
                typename I = range_iterator_t<Rng>,
                CONCEPT_REQUIRES_(Iterable<Rng &>() && BinarySearchable<I, V2, C, P>())>
            I operator()(Rng &rng, V2 const &val, C pred = C{}, P proj = P{}) const
            {
                static_assert(!is_infinite<Rng>::value, "Trying to binary search an infinite range");
                return aux::upper_bound_n(begin(rng), distance(rng), val, std::move(pred),
                    std::move(proj));
            }
        };

        /// \sa `upper_bound_fn`
        /// \ingroup group-algorithms
        constexpr upper_bound_fn upper_bound{};

        /// @}
    } // namespace v3
} // namespace ranges

#endif // include guard
