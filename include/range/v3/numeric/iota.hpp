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
#ifndef RANGES_V3_NUMERIC_IOTA_HPP
#define RANGES_V3_NUMERIC_IOTA_HPP

#include <range/v3/begin_end.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    inline namespace v3
    {
        struct iota_fn
        {
            template<typename O, typename S, typename T,
                CONCEPT_REQUIRES_(OutputIterator<O, T const &>() && Sentinel<S, O>() &&
                    WeaklyIncrementable<T>())>
            O operator()(O begin, S end, T val) const
            {
                for(; begin != end; ++begin, ++val)
                    *begin = detail::as_const(val);
                return begin;
            }

            template<typename Rng, typename T,
                CONCEPT_REQUIRES_(OutputRange<Rng, T const &>() && WeaklyIncrementable<T>())>
            safe_iterator_t<Rng> operator()(Rng && rng, T val) const
            {
                return (*this)(begin(rng), end(rng), detail::move(val));
            }
        };

        RANGES_INLINE_VARIABLE(iota_fn, iota)
    }
}

#endif
