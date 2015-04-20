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
#ifndef RANGES_V3_ALGORITHM_ROTATE_COPY_HPP
#define RANGES_V3_ALGORITHM_ROTATE_COPY_HPP

#include <functional>
#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/algorithm/copy.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-algorithms
        /// @{
        struct rotate_copy_fn
        {
            template<typename I, typename S, typename O, typename P = ident,
                CONCEPT_REQUIRES_(ForwardIterator<I>() && IteratorRange<I, S>() && WeaklyIncrementable<O>() &&
                    IndirectlyCopyable<I, O>())>
            RANGES_RELAXED_CONSTEXPR
            std::pair<I, O> operator()(I begin, I middle, S end, O out) const
            {
                auto res = copy(middle, std::move(end), std::move(out));
                return {
                    std::move(res.first),
                    copy(std::move(begin), middle, std::move(res.second)).second
                };
            }

            template<typename Rng, typename O, typename P = ident,
                typename I = range_iterator_t<Rng>,
                CONCEPT_REQUIRES_(Iterable<Rng>() && WeaklyIncrementable<O>() &&
                    IndirectlyCopyable<I, O>())>
            RANGES_RELAXED_CONSTEXPR
            std::pair<range_safe_iterator_t<Rng>, O>
            operator()(Rng &&rng, I middle, O out) const
            {
                return (*this)(begin(rng), std::move(middle), end(rng), std::move(out));
            }
        };

        /// \sa `rotate_copy_fn`
        /// \ingroup group-algorithms
        namespace
        {
            constexpr auto&& rotate_copy = static_const<with_braced_init_args<rotate_copy_fn>>::value;
        }

        /// @}
    } // namespace v3
} // namespace ranges

#endif // include guard
