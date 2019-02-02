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

#ifndef RANGES_V3_ACTION_PUSH_FRONT_HPP
#define RANGES_V3_ACTION_PUSH_FRONT_HPP

#include <utility>
#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/action/insert.hpp>
#include <range/v3/action/action.hpp>
#include <range/v3/detail/with_braced_init_args.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    /// \cond
    namespace adl_push_front_detail
    {
        template<typename Cont, typename T>
        using push_front_t =
            decltype(static_cast<void>(unwrap_reference(
                std::declval<Cont &>()).push_front(std::declval<T>())));

        template<typename Cont, typename Rng>
        using insert_t =
            decltype(static_cast<void>(ranges::insert(
                std::declval<Cont &>(),
                std::declval<iterator_t<Cont>>(),
                std::declval<Rng>())));

        CPP_template(typename Cont, typename T)(
            requires LvalueContainerLike<Cont> && !Range<T> &&
                Constructible<range_value_t<Cont>, T>)
        push_front_t<Cont, T> push_front(Cont &&cont, T &&t)
        {
            unwrap_reference(cont).push_front(static_cast<T &&>(t));
        }

        CPP_template(typename Cont, typename Rng)(
            requires LvalueContainerLike<Cont> && Range<Rng>)
        insert_t<Cont, Rng> push_front(Cont &&cont, Rng &&rng)
        {
            ranges::insert(cont, begin(cont), static_cast<Rng &&>(rng));
        }

        CPP_def
        (
            template(typename Rng, typename T)
            concept PushFrontActionConcept,
                requires (Rng &&rng, T &&t)
                (
                    push_front(rng, (T &&) t)
                ) &&
                InputRange<Rng> &&
                    (Range<T> || Constructible<range_value_t<Rng>, T>)
        );

        struct push_front_fn
        {
        private:
            friend action::action_access;
            template<typename T>
            static auto bind(push_front_fn push_front, T &&val)
            {
                return std::bind(push_front, std::placeholders::_1, bind_forward<T>(val));
            }
        public:
            CPP_template(typename Rng, typename T)(
                requires PushFrontActionConcept<Rng, T>)
            Rng operator()(Rng &&rng, T &&t) const
            {
                push_front(rng, static_cast<T &&>(t));
                return static_cast<Rng &&>(rng);
            }
        };
    }
    /// \endcond

    namespace action
    {
        /// \ingroup group-actions
        RANGES_INLINE_VARIABLE(
            detail::with_braced_init_args<action<adl_push_front_detail::push_front_fn>>,
            push_front)
    }

    using action::push_front;
}

#endif
