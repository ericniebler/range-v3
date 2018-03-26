/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2014-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
#ifndef RANGES_V3_ALGORITHM_REMOVE_COPY_IF_HPP
#define RANGES_V3_ALGORITHM_REMOVE_COPY_IF_HPP

#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/utility/tagged_pair.hpp>
#include <range/v3/algorithm/tagspec.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \ingroup group-concepts
        template<typename I, typename O, typename C, typename P = ident>
        using RemoveCopyableIf = meta::strict_and<
            InputIterator<I>,
            WeaklyIncrementable<O>,
            IndirectPredicate<C, projected<I, P>>,
            IndirectlyCopyable<I, O>>;

        /// \addtogroup group-algorithms
        /// @{
        struct remove_copy_if_fn
        {
            template<typename I, typename S, typename O, typename C, typename P = ident,
                CONCEPT_REQUIRES_(RemoveCopyableIf<I, O, C, P>() && Sentinel<S, I>())>
            tagged_pair<tag::in(I), tag::out(O)> operator()(I begin, S end, O out, C pred, P proj = P{}) const
            {
                for(; begin != end; ++begin)
                {
                    auto &&x = *begin;
                    if(!(invoke(pred, invoke(proj, x))))
                    {
                        *out = (decltype(x) &&) x;
                        ++out;
                    }
                }
                return {begin, out};
            }

            template<typename Rng, typename O, typename C, typename P = ident,
                typename I = iterator_t<Rng>,
                CONCEPT_REQUIRES_(RemoveCopyableIf<I, O, C, P>() && InputRange<Rng>())>
            tagged_pair<tag::in(safe_iterator_t<Rng>), tag::out(O)> operator()(Rng &&rng, O out, C pred, P proj = P{}) const
            {
                return (*this)(begin(rng), end(rng), std::move(out), std::move(pred), std::move(proj));
            }
        };

        /// \sa `remove_copy_if_fn`
        /// \ingroup group-algorithms
        RANGES_INLINE_VARIABLE(with_braced_init_args<remove_copy_if_fn>,
                               remove_copy_if)
        /// @}
    } // namespace v3
} // namespace ranges

#endif // include guard
