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

#ifndef RANGES_V3_ACTION_ERASE_HPP
#define RANGES_V3_ACTION_ERASE_HPP

#include <utility>
#include <range/v3/range_fwd.hpp>
#include <range/v3/action/insert.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \cond
        namespace adl_erase_detail
        {
            CONCEPT_template(typename Cont, typename I, typename S)(
                requires LvalueContainerLike<Cont> && ForwardIterator<I> &&
                    Sentinel<S, I>)
            decltype(unwrap_reference(std::declval<Cont &>()).erase(std::declval<I>(), std::declval<S>()))
            erase(Cont &&cont, I begin, S end)
            {
                return unwrap_reference(cont).erase(begin, end);
            }

            struct erase_fn
            {
                CONCEPT_template(typename Rng, typename I, typename S)(
                    requires Range<Rng> && ForwardIterator<I> &&
                        Sentinel<S, I>)
                decltype(erase(std::declval<Rng>(), std::declval<I>(), std::declval<S>()))
                operator()(Rng &&rng, I begin, S end) const
                {
                    return erase(static_cast<Rng &&>(rng), begin, end);
                }
            };
        }
        /// \endcond

        /// \ingroup group-actions
        RANGES_INLINE_VARIABLE(adl_erase_detail::erase_fn, erase)

        namespace action
        {
            using ranges::erase;
        }

        /// \addtogroup group-concepts
        /// @{
        /// \ingroup group-concepts
        CONCEPT_def
        (
            template(typename Rng, typename I, typename S)
            concept ErasableRange,
                requires (Rng &&rng, I begin, S end)
                (
                    ranges::erase(static_cast<Rng &&>(rng), begin, end)
                ) &&
                Range<Rng>
        );
        /// @}
    }
}

#endif
