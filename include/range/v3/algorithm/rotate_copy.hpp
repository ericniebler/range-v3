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
#include <range/v3/utility/tagged_pair.hpp>
#include <range/v3/algorithm/tagspec.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-algorithms
        /// @{
        struct rotate_copy_fn
        {
            template<typename I, typename S, typename O, typename P = ident,
                CONCEPT_REQUIRES_(ForwardIterator<I>() && Sentinel<S, I>() && WeaklyIncrementable<O>() &&
                    IndirectlyCopyable<I, O>())>
            tagged_pair<tag::in(I), tag::out(O)> operator()(I begin, I middle, S end, O out) const
            {
                auto res = copy(middle, std::move(end), std::move(out));
                return {
                    std::move(res.first),
                    copy(std::move(begin), middle, std::move(res.second)).second
                };
            }

            template<typename Rng, typename O, typename P = ident,
                typename I = iterator_t<Rng>,
                CONCEPT_REQUIRES_(Range<Rng>() && WeaklyIncrementable<O>() &&
                    IndirectlyCopyable<I, O>())>
            tagged_pair<tag::in(safe_iterator_t<Rng>), tag::out(O)>
            operator()(Rng &&rng, I middle, O out) const
            {
                return (*this)(begin(rng), std::move(middle), end(rng), std::move(out));
            }
        };

        /// \sa `rotate_copy_fn`
        /// \ingroup group-algorithms
        RANGES_INLINE_VARIABLE(with_braced_init_args<rotate_copy_fn>, rotate_copy)
        /// @}
    } // namespace v3
} // namespace ranges

#endif // include guard
