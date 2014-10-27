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
#include <range/v3/utility/bindable.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace adl_push_back_detail
        {
            template<typename Cont, typename T,
                CONCEPT_REQUIRES_(Container<Cont>() && Constructible<range_value_t<Cont>, T &&>())>
            auto push_back(Cont & cont, T && t) ->
                decltype((void)cont.push_back(std::forward<T>(t)))
            {
                cont.push_back(std::forward<T>(t));
            }

            template<typename Cont, typename Rng,
                CONCEPT_REQUIRES_(Container<Cont>() && Iterable<Rng>())>
            auto push_back(Cont & cont, Rng && rng) ->
                decltype((void)container::insert(cont, end(cont), std::forward<Rng>(rng)))
            {
                container::insert(cont, end(cont), std::forward<Rng>(rng));
            }

            struct push_back_impl_fn : bindable<push_back_impl_fn>
            {
                template<typename Cont, typename T,
                    CONCEPT_REQUIRES_(Container<Cont>() && Constructible<range_value_t<Cont>, T &&>())>
                static auto invoke(push_back_impl_fn, Cont & cont, T && t) ->
                    decltype((void)push_back(cont, std::forward<T>(t)))
                {
                    return push_back(cont, std::forward<T>(t));
                }
                template<typename Cont, typename Rng,
                    CONCEPT_REQUIRES_(Container<Cont>() && Iterable<Rng>())>
                static auto invoke(push_back_impl_fn, Cont & cont, Rng && rng) ->
                    decltype((void)push_back(cont, std::forward<Rng>(rng)))
                {
                    return push_back(cont, std::forward<Rng>(rng));
                }
                template<typename Cont,
                    CONCEPT_REQUIRES_(Container<Cont>())>
                static auto invoke(push_back_impl_fn push_back, Cont & cont) ->
                    decltype(push_back.move_bind(cont, std::placeholders::_1))
                {
                    return push_back.move_bind(cont, std::placeholders::_1);
                }
            };

            struct push_back_fn : push_back_impl_fn
            {
                using push_back_impl_fn::operator();

                template<typename Cont, typename T,
                    CONCEPT_REQUIRES_(Container<Cont>())>
                auto operator()(Cont & cont, std::initializer_list<T> t) const ->
                    decltype(push_back_impl_fn{}(cont, t))
                {
                    return push_back_impl_fn{}(cont, t);
                }
            };
        }

        namespace container
        {
            RANGES_CONSTEXPR adl_push_back_detail::push_back_fn push_back{};
        }
    }
}

#endif
