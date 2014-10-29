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

#ifndef RANGES_V3_CONTAINER_PUSH_BACK_HPP
#define RANGES_V3_CONTAINER_PUSH_BACK_HPP

#include <utility>
#include <initializer_list>
#include <range/v3/range_fwd.hpp>
#include <range/v3/container/insert.hpp>
#include <range/v3/utility/pipeable.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace adl_push_back_detail
        {
            template<typename Cont, typename T,
                CONCEPT_REQUIRES_(LvalueContainerLike<Cont>() && Constructible<range_value_t<Cont>, T &&>())>
            auto push_back(Cont && cont, T && t) ->
                decltype((void)unwrap_reference(cont).push_back(std::forward<T>(t)))
            {
                unwrap_reference(cont).push_back(std::forward<T>(t));
            }

            template<typename Cont, typename Rng,
                CONCEPT_REQUIRES_(LvalueContainerLike<Cont>() && Iterable<Rng>())>
            auto push_back(Cont && cont, Rng && rng) ->
                decltype((void)ranges::insert(unwrap_reference(cont), end(cont), std::forward<Rng>(rng)))
            {
                ranges::insert(unwrap_reference(cont), end(cont), std::forward<Rng>(rng));
            }

            struct push_back_fn
            {
                template<typename Rng, typename T,
                    CONCEPT_REQUIRES_(Iterable<Rng>() && Constructible<range_value_t<Rng>, T &&>())>
                auto operator()(Rng && rng, T && t) const ->
                    decltype((void)push_back(std::forward<Rng>(rng), std::forward<T>(t)))
                {
                    push_back(std::forward<Rng>(rng), std::forward<T>(t));
                }
                template<typename Rng, typename Rng2,
                    CONCEPT_REQUIRES_(Iterable<Rng>() && Iterable<Rng2>())>
                auto operator()(Rng && rng, Rng2 && rng2) const ->
                    decltype((void)push_back(std::forward<Rng>(rng), std::forward<Rng2>(rng2)))
                {
                    push_back(std::forward<Rng>(rng), std::forward<Rng2>(rng2));
                }
                template<typename Rng, typename T,
                    CONCEPT_REQUIRES_(Iterable<Rng>())>
                auto operator()(Rng && rng, std::initializer_list<T> rng2) const ->
                    decltype((void)push_back(std::forward<Rng>(rng), rng2))
                {
                    push_back(std::forward<Rng>(rng), rng2);
                }
                template<typename Rng,
                    CONCEPT_REQUIRES_(Iterable<Rng>())>
                auto operator()(Rng && rng) const ->
                    decltype(pipeable_bind(*this, bind_forward<Rng>(rng), std::placeholders::_1))
                {
                    return pipeable_bind(*this, bind_forward<Rng>(rng), std::placeholders::_1);
                }
            };
        }

        RANGES_CONSTEXPR adl_push_back_detail::push_back_fn push_back{};
    }
}

#endif
