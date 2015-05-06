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
#ifndef RANGES_V3_ALGORITHM_REVERSE_COPY_HPP
#define RANGES_V3_ALGORITHM_REVERSE_COPY_HPP

#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/iterator.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \ingroup group-concepts
        template<typename I, typename O>
        using ReverseCopyable = meta::fast_and<
            BidirectionalIterator<I>,
            WeaklyIncrementable<O>,
            IndirectlyCopyable<I, O>>;

        /// \addtogroup group-algorithms
        /// @{
        struct reverse_copy_fn
        {
            template<typename I, typename S, typename O,
                CONCEPT_REQUIRES_(IteratorRange<I, S>() && ReverseCopyable<I, O>())>
            RANGES_CXX14_CONSTEXPR
            std::pair<I, O> operator()(I begin, S end_, O out) const
            {
                I end = ranges::next(begin, end_), res = end;
                for (; begin != end; ++out)
                    *out = *--end;
                return {res, out};
            }

            template<typename Rng, typename O,
                typename I = range_iterator_t<Rng>,
                CONCEPT_REQUIRES_(Iterable<Rng>() && ReverseCopyable<I, O>())>
            RANGES_CXX14_CONSTEXPR
            std::pair<range_safe_iterator_t<Rng>, O> operator()(Rng &&rng, O out) const
            {
                return (*this)(begin(rng), end(rng), std::move(out));
            }
        };

        /// \sa `reverse_copy_fn`
        /// \ingroup group-algorithms
        namespace
        {
            constexpr auto&& reverse_copy = static_const<with_braced_init_args<reverse_copy_fn>>::value;
        }

        /// @}
    } // namespace v3
} // namespace ranges

#endif // include guard
