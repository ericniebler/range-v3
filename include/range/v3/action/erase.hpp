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
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    /// \cond
    namespace adl_erase_detail
    {
        CPP_template(typename Cont, typename I, typename S)(
            requires LvalueContainerLike<Cont> && ForwardIterator<I> &&
                Sentinel<S, I>)
        auto erase(Cont &&cont, I begin, S end) ->
            decltype(unwrap_reference(cont).erase(begin, end))
        {
            return unwrap_reference(cont).erase(begin, end);
        }

        struct erase_fn
        {
            CPP_template(typename Rng, typename I, typename S)(
                requires Range<Rng> && ForwardIterator<I> &&
                    Sentinel<S, I>)
            auto operator()(Rng &&rng, I begin, S end) const ->
                decltype(erase((Rng &&) rng, begin, end))
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
    CPP_def
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

#endif
