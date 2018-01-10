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
#ifndef RANGES_V3_ALGORITHM_REPLACE_COPY_IF_HPP
#define RANGES_V3_ALGORITHM_REPLACE_COPY_IF_HPP

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
        template<typename I, typename O, typename C, typename T, typename P = ident>
        using ReplaceCopyIfable = meta::strict_and<
            InputIterator<I>,
            OutputIterator<O, T const &>,
            IndirectlyCopyable<I, O>,
            IndirectPredicate<C, projected<I, P>>>;

        /// \addtogroup group-algorithms
        /// @{
        struct replace_copy_if_fn
        {
            template<typename I, typename S, typename O, typename C, typename T, typename P = ident,
                CONCEPT_REQUIRES_(ReplaceCopyIfable<I, O, C, T, P>() && Sentinel<S, I>())>
            tagged_pair<tag::in(I), tag::out(O)> operator()(I begin, S end, O out, C pred, T const & new_value, P proj = {}) const
            {
                for(; begin != end; ++begin, ++out)
                {
                    auto &&x = *begin;
                    if(invoke(pred, invoke(proj, x)))
                        *out = new_value;
                    else
                        *out = (decltype(x) &&) x;
                }
                return {begin, out};
            }

            template<typename Rng, typename O, typename C, typename T, typename P = ident,
                typename I = iterator_t<Rng>,
                CONCEPT_REQUIRES_(ReplaceCopyIfable<I, O, C, T, P>() && Range<Rng>())>
            tagged_pair<tag::in(safe_iterator_t<Rng>), tag::out(O)>
            operator()(Rng &&rng, O out, C pred, T const & new_value, P proj = {}) const
            {
                return (*this)(begin(rng), end(rng), std::move(out), std::move(pred), new_value,
                    std::move(proj));
            }
        };

        /// \sa `replace_copy_if_fn`
        /// \ingroup group-algorithms
        RANGES_INLINE_VARIABLE(with_braced_init_args<replace_copy_if_fn>,
                               replace_copy_if)
        /// @}
    } // namespace v3
} // namespace ranges

#endif // include guard
