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
#ifndef RANGES_V3_ALGORITHM_REMOVE_IF_HPP
#define RANGES_V3_ALGORITHM_REMOVE_IF_HPP

#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/iterator.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/algorithm/find_if.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \ingroup group-concepts
        template<typename I, typename C, typename P = ident>
        using RemovableIf = meta::strict_and<
            ForwardIterator<I>,
            IndirectPredicate<C, projected<I, P>>,
            Permutable<I>>;

        /// \addtogroup group-algorithms
        /// @{
        struct remove_if_fn
        {
            template<typename I, typename S, typename C, typename P = ident,
                CONCEPT_REQUIRES_(RemovableIf<I, C, P>() && Sentinel<S, I>())>
            I operator()(I begin, S end, C pred, P proj = P{}) const
            {
                begin = find_if(std::move(begin), end, std::ref(pred), std::ref(proj));
                if(begin != end)
                {
                    for(I i = next(begin); i != end; ++i)
                    {
                        if(!(invoke(pred, invoke(proj, *i))))
                        {
                            *begin = iter_move(i);
                            ++begin;
                        }
                    }
                }
                return begin;
            }

            template<typename Rng, typename C, typename P = ident,
                typename I = iterator_t<Rng>,
                CONCEPT_REQUIRES_(RemovableIf<I, C, P>() && ForwardRange<Rng>())>
            safe_iterator_t<Rng> operator()(Rng &&rng, C pred, P proj = P{}) const
            {
                return (*this)(begin(rng), end(rng), std::move(pred), std::move(proj));
            }
        };

        /// \sa `remove_if_fn`
        /// \ingroup group-algorithms
        RANGES_INLINE_VARIABLE(with_braced_init_args<remove_if_fn>, remove_if)
        /// @}
    } // namespace v3
} // namespace ranges

#endif // include guard
