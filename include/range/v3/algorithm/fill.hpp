//  Copyright Neil Groves 2009.
//  Copyright Eric Niebler 2013
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//
#ifndef RANGES_V3_ALGORITHM_FILL_HPP
#define RANGES_V3_ALGORITHM_FILL_HPP

#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>

namespace ranges
{
    inline namespace v3
    {
        struct fill_fn
        {
            //// BUGBUG Sentinel requires InputIterator
            //template<typename O, typename S, typename V,
            //    CONCEPT_REQUIRES_(OutputIterator<O, T>() && Sentinel<S, O>())>
            //O operator()(O begin, S end, V const & val) const
            //{
            //    for(; begin != end; ++begin)
            //        *begin = val;
            //    return begin;
            //}

            //// BUGBUG overconstrained, Iterable requires WeakInputIterators
            //template<typename Rng, typename V,
            //    typename O = ranges_iterator_t<Rng>,
            //    CONCEPT_REQUIRES_(Iterable<Rng>() && OutputIterator<O, T>())>
            //O operator()(O begin, O end, V const & val) const
            //{
            //    return (*this)(begin(rng), end(rng), val);
            //}
        };

        RANGES_CONSTEXPR fill_fn fill{};

    } // namespace v3
} // namespace ranges

#endif // include guard
