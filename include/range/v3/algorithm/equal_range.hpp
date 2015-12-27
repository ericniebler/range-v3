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
#ifndef RANGES_V3_ALGORITHM_EQUAL_RANGE_HPP
#define RANGES_V3_ALGORITHM_EQUAL_RANGE_HPP

#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/distance.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/iterator_range.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/algorithm/aux_/equal_range_n.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-algorithms
        /// @{
        struct equal_range_fn
        {
            template<typename I, typename S, typename V, typename C = ordered_less, typename P = ident,
                CONCEPT_REQUIRES_(IteratorRange<I, S>() && BinarySearchable<I, V, C, P>())>
            iterator_range<I>
            operator()(I begin, S end, V const & val, C pred = C{}, P proj = P{}) const
            {
                return aux::equal_range_n(std::move(begin), distance(begin, end), val, std::move(pred),
                    std::move(proj));
            }

            template<typename Rng, typename V, typename C = ordered_less, typename P = ident,
                typename I = range_iterator_t<Rng>,
                CONCEPT_REQUIRES_(Range<Rng>() && BinarySearchable<I, V, C, P>())>
            meta::if_<std::is_lvalue_reference<Rng>, iterator_range<I>, dangling<iterator_range<I>>>
            operator()(Rng &&rng, V const & val, C pred = C{}, P proj = P{}) const
            {
                static_assert(!is_infinite<Rng>::value, "Trying to binary search an infinite range");
                return aux::equal_range_n(begin(rng), distance(rng), val, std::move(pred),
                    std::move(proj));
            }
        };

        /// \sa `equal_range_fn`
        /// \ingroup group-algorithms
        namespace
        {
            constexpr auto&& equal_range = static_const<with_braced_init_args<equal_range_fn>>::value;
        }

        /// @}
    } // namespace v3
} // namespace ranges

#endif // include guard
