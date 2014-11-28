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

#ifndef RANGES_V3_ACTION_ERASE_HPP
#define RANGES_V3_ACTION_ERASE_HPP

#include <utility>
#include <range/v3/range_fwd.hpp>
#include <range/v3/action/insert.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/pipeable.hpp>

namespace ranges
{
    inline namespace v3
    {
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
                    IteratorRange<I, S>())>
            auto erase(Cont && cont, I begin, S end) ->
                decltype(unwrap_reference(cont).erase(begin, end))
            {
                return unwrap_reference(cont).erase(begin, end);
            }

            struct erase_fn
            {
                // TODO associative erase by key
                template<typename Rng, typename I,
                    CONCEPT_REQUIRES_(Iterable<Rng>() && ForwardIterator<I>())>
                auto operator()(Rng && rng, I it) const ->
                    decltype(erase(std::forward<Rng>(rng), it))
                {
                    return erase(std::forward<Rng>(rng), it);
                }
                template<typename Rng, typename I, typename S,
                    CONCEPT_REQUIRES_(Iterable<Rng>() && ForwardIterator<I>() &&
                        IteratorRange<I, S>())>
                auto operator()(Rng && rng, I begin, S end) const ->
                    decltype(erase(std::forward<Rng>(rng), begin, end))
                {
                    return erase(std::forward<Rng>(rng), begin, end);
                }
            };
        }

        constexpr adl_erase_detail::erase_fn erase{};

        namespace action
        {
            using ranges::erase;
        }

        namespace concepts
        {
            struct EraseableIterable
              : refines<Iterable(_1)>
            {
                template<typename Rng, typename...Rest>
                using result_t = decltype(ranges::erase(val<Rng>(), val<Rest>()...));

                template<typename Rng, typename...Rest>
                auto requires_(Rng rng, Rest... rest) -> decltype(
                    concepts::valid_expr(
                        (ranges::erase(val<Rng>(), val<Rest>()...), 42)
                    ));
            };
        }

        template<typename Rng, typename...Rest>
        using EraseableIterable = concepts::models<concepts::EraseableIterable, Rng, Rest...>;
    }
}

#endif
