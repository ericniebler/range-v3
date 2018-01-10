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
            template<typename Cont, typename I,
                CONCEPT_REQUIRES_(LvalueContainerLike<Cont>() && ForwardIterator<I>())>
            auto erase(Cont && cont, I it) ->
                decltype(unwrap_reference(cont).erase(it))
            {
                return unwrap_reference(cont).erase(it);
            }

            template<typename Cont, typename I, typename S,
                CONCEPT_REQUIRES_(LvalueContainerLike<Cont>() && ForwardIterator<I>() &&
                    Sentinel<S, I>())>
            auto erase(Cont && cont, I begin, S end) ->
                decltype(unwrap_reference(cont).erase(begin, end))
            {
                return unwrap_reference(cont).erase(begin, end);
            }

            struct erase_fn
            {
                // TODO associative erase by key
                template<typename Rng, typename I,
                    CONCEPT_REQUIRES_(Range<Rng>() && ForwardIterator<I>())>
                auto operator()(Rng && rng, I it) const ->
                    decltype(erase(static_cast<Rng&&>(rng), it))
                {
                    return erase(static_cast<Rng&&>(rng), it);
                }
                template<typename Rng, typename I, typename S,
                    CONCEPT_REQUIRES_(Range<Rng>() && ForwardIterator<I>() &&
                        Sentinel<S, I>())>
                auto operator()(Rng && rng, I begin, S end) const ->
                    decltype(erase(static_cast<Rng&&>(rng), begin, end))
                {
                    return erase(static_cast<Rng&&>(rng), begin, end);
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
        namespace concepts
        {
            struct ErasableRange
              : refines<Range(_1)>
            {
                template<typename Rng, typename...Rest>
                using result_t =
                    decltype(ranges::erase(std::declval<Rng&>(), std::declval<Rest>()...));

                template<typename Rng, typename...Rest>
                auto requires_() ->
                    meta::void_<result_t<Rng, Rest...>>;
            };
        }

        template<typename Rng, typename...Rest>
        using ErasableRange = concepts::models<concepts::ErasableRange, Rng, Rest...>;
        /// @}
    }
}

#endif
