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
#ifndef RANGES_V3_ALGORITHM_REPLACE_IF_HPP
#define RANGES_V3_ALGORITHM_REPLACE_IF_HPP

#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \ingroup group-concepts
        template<typename I, typename C, typename T, typename P = ident>
        using ReplaceIfable = meta::strict_and<
            InputIterator<I>,
            IndirectPredicate<C, projected<I, P>>,
            Writable<I, T const &>>;

        /// \addtogroup group-algorithms
        /// @{
        struct replace_if_fn
        {
            template<typename I, typename S, typename C, typename T, typename P = ident,
                CONCEPT_REQUIRES_(ReplaceIfable<I, C, T, P>() && Sentinel<S, I>())>
            I operator()(I begin, S end, C pred, T const & new_value, P proj = P{}) const
            {
                for(; begin != end; ++begin)
                    if(invoke(pred, invoke(proj, *begin)))
                        *begin = new_value;
                return begin;
            }

            template<typename Rng, typename C, typename T, typename P = ident,
                typename I = iterator_t<Rng>,
                CONCEPT_REQUIRES_(ReplaceIfable<I, C, T, P>() && Range<Rng>())>
            safe_iterator_t<Rng>
            operator()(Rng &&rng, C pred, T const & new_value, P proj = P{}) const
            {
                return (*this)(begin(rng), end(rng), std::move(pred), new_value, std::move(proj));
            }
        };

        /// \sa `replace_if_fn`
        /// \ingroup group-algorithms
        RANGES_INLINE_VARIABLE(with_braced_init_args<replace_if_fn>, replace_if)
        /// @}
    } // namespace v3
} // namespace ranges

#endif // include guard
