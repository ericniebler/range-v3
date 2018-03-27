/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2014-present
//  Copyright Casey Carter 2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
#ifndef RANGES_V3_ALGORITHM_MAX_HPP
#define RANGES_V3_ALGORITHM_MAX_HPP

#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/iterator.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-algorithms
        /// @{
        struct max_fn
        {
            template<typename Rng, typename C = ordered_less, typename P = ident,
                typename I = iterator_t<Rng>, typename V = value_type_t<I>,
                CONCEPT_REQUIRES_(InputRange<Rng>() && Copyable<V>() &&
                    IndirectRelation<C, projected<I, P>>())>
            RANGES_CXX14_CONSTEXPR V operator()(Rng &&rng, C pred = C{}, P proj = P{}) const
            {
                auto begin = ranges::begin(rng);
                auto end = ranges::end(rng);
                RANGES_EXPECT(begin != end);
                V result = *begin;
                while(++begin != end)
                {
                    auto && tmp = *begin;
                    if(invoke(pred, invoke(proj, result), invoke(proj, tmp)))
                        result = (decltype(tmp) &&) tmp;
                }
                return result;
            }

            template<typename T, typename C = ordered_less, typename P = ident,
                CONCEPT_REQUIRES_(
                    IndirectRelation<C, projected<const T *, P>>())>
            constexpr T const &operator()(T const &a, T const &b, C pred = C{}, P proj = P{}) const
            {
                return invoke(pred, invoke(proj, b), invoke(proj, a)) ? a : b;
            }
        };

        /// \sa `max_fn`
        /// \ingroup group-algorithms
        RANGES_INLINE_VARIABLE(with_braced_init_args<max_fn>, max)
        /// @}
    } // namespace v3
} // namespace ranges

#endif // include guard
