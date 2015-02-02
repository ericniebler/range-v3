/// \file
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
#ifndef RANGES_V3_ALGORITHM_COPY_HPP
#define RANGES_V3_ALGORITHM_COPY_HPP

#include <utility>
#include <functional>
#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/copy.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-algorithms
        /// @{
        struct copy_fn : aux::copy_fn
        {
            using aux::copy_fn::operator();

            template<typename I, typename S, typename O,
                CONCEPT_REQUIRES_(
                    InputIterator<I>() && IteratorRange<I, S>() &&
                    WeaklyIncrementable<O>() &&
                    IndirectlyCopyable<I, O>()
                )>
            std::pair<I, O>
            operator()(I begin, S end, O out) const
            {
                for(; begin != end; ++begin, ++out)
                    *out = *begin;
                return {begin, out};
            }

            template<typename Rng, typename O,
                typename I = range_iterator_t<Rng>,
                CONCEPT_REQUIRES_(
                    InputIterable<Rng &>() &&
                    WeaklyIncrementable<O>() &&
                    IndirectlyCopyable<I, O>()
                )>
            std::pair<I, O>
            operator()(Rng &rng, O out) const
            {
                return (*this)(begin(rng), end(rng), std::move(out));
            }
        };

        /// \sa `copy_fn`
        /// \ingroup group-algorithms
        namespace
        {
            constexpr auto&& copy = static_const<copy_fn>::value;
        }

        /// @}
    } // namespace v3
} // namespace ranges

#endif // include guard
