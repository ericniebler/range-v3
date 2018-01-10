/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2013-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
#ifndef RANGES_V3_ALGORITHM_FILL_HPP
#define RANGES_V3_ALGORITHM_FILL_HPP

#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-algorithms
        /// @{
        struct fill_fn
        {
            template<typename O, typename S, typename V,
                CONCEPT_REQUIRES_(OutputIterator<O, V const &>() && Sentinel<S, O>())>
            O operator()(O begin, S end, V const & val) const
            {
                for(; begin != end; ++begin)
                    *begin = val;
                return begin;
            }

            template<typename Rng, typename V,
                typename O = iterator_t<Rng>,
                CONCEPT_REQUIRES_(OutputRange<Rng, V const &>())>
            safe_iterator_t<Rng> operator()(Rng &&rng, V const & val) const
            {
                return (*this)(begin(rng), end(rng), val);
            }
        };

        /// \sa `fill_fn`
        /// \ingroup group-algorithms
        RANGES_INLINE_VARIABLE(with_braced_init_args<fill_fn>, fill)
        /// @}
    } // namespace v3
} // namespace ranges

#endif // include guard
