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

#ifndef RANGES_V3_CONTAINER_PUSH_FRONT_HPP
#define RANGES_V3_CONTAINER_PUSH_FRONT_HPP

#include <utility>
#include <initializer_list>
#include <range/v3/range_fwd.hpp>
#include <range/v3/container/insert.hpp>
#include <range/v3/utility/bindable.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace adl_push_front_detail
        {
            template<typename Cont, typename T,
                CONCEPT_REQUIRES_(Container<Cont>() && Constructible<range_value_t<Cont>, T &&>())>
            auto push_front(Cont & cont, T && t) ->
                decltype((void)cont.push_front(std::forward<T>(t)))
            {
                cont.push_front(std::forward<T>(t));
            }

            template<typename Cont, typename Rng,
                CONCEPT_REQUIRES_(Container<Cont>() && Iterable<Rng>())>
            auto push_front(Cont & cont, Rng && rng) ->
                decltype((void)container::insert(cont, begin(cont), std::forward<Rng>(rng)))
            {
                container::insert(cont, begin(cont), std::forward<Rng>(rng));
            }

            struct push_front_impl_fn : bindable<push_front_impl_fn>
            {
                template<typename Cont, typename T,
                    CONCEPT_REQUIRES_(Container<Cont>() && Constructible<range_value_t<Cont>, T &&>())>
                static auto invoke(push_front_impl_fn, Cont & cont, T && t) ->
                    decltype((void)push_front(cont, std::forward<T>(t)))
                {
                    return push_front(cont, std::forward<T>(t));
                }
                template<typename Cont, typename Rng,
                    CONCEPT_REQUIRES_(Container<Cont>() && Iterable<Rng>())>
                static auto invoke(push_front_impl_fn, Cont & cont, Rng && rng) ->
                    decltype((void)push_front(cont, std::forward<Rng>(rng)))
                {
                    return push_front(cont, std::forward<Rng>(rng));
                }
                template<typename Cont,
                    CONCEPT_REQUIRES_(Container<Cont>())>
                static auto invoke(push_front_impl_fn push_front, Cont & cont) ->
                    decltype(push_front.move_bind(cont, std::placeholders::_1))
                {
                    return push_front.move_bind(cont, std::placeholders::_1);
                }
            };

            struct push_front_fn : push_front_impl_fn
            {
                using push_front_impl_fn::operator();

                template<typename Cont, typename T,
                    CONCEPT_REQUIRES_(Container<Cont>())>
                auto operator()(Cont & cont, std::initializer_list<T> t) const ->
                    decltype(push_front_impl_fn{}(cont, t))
                {
                    return push_front_impl_fn{}(cont, t);
                }
            };
        }

        namespace container
        {
            RANGES_CONSTEXPR adl_push_front_detail::push_front_fn push_front{};
        }
    }
}

#endif
