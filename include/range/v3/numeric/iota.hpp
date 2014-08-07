///////////////////////////////////////////////////////////////////////////////
/// \file accumulate.hpp
///   Contains range-based versions of the std generic numeric operation:
///     iota.
//
// Copyright 2004 Eric Niebler.
// Copyright 2006 Thorsten Ottosen.
// Copyright 2009 Neil Groves.
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef RANGES_V3_NUMERIC_IOTA_HPP
#define RANGES_V3_NUMERIC_IOTA_HPP

#include <range/v3/begin_end.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/utility/iterator_concepts.hpp>

namespace ranges
{
    inline namespace v3
    {
        struct iota_fn
        {
            template<typename O, typename S, class T,
                CONCEPT_REQUIRES_(OutputIterator<O, T, S>() && WeaklyIncrementable<T>())>
            O operator()(O begin, S end, T val) const
            {
                for(; begin != end; ++begin, ++val)
                    *begin = val;
                return begin;
            }

            template<typename Rng, class T, typename O = range_iterator_t<Rng>,
                CONCEPT_REQUIRES_(OutputIterable<Rng, T>() && WeaklyIncrementable<T>())>
            O operator()(Rng &rng, T val) const
            {
                return (*this)(begin(rng), end(rng), std::move(val));
            }
        };

        RANGES_CONSTEXPR iota_fn iota{};
    }
}

#endif
