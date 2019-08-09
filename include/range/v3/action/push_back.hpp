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

#ifndef RANGES_V3_ACTION_PUSH_BACK_HPP
#define RANGES_V3_ACTION_PUSH_BACK_HPP

#include <utility>

#include <meta/meta.hpp>

#include <range/v3/range_fwd.hpp>

#include <range/v3/action/action.hpp>
#include <range/v3/action/insert.hpp>
#include <range/v3/detail/with_braced_init_args.hpp>
#include <range/v3/functional/bind_back.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    /// \cond
    namespace adl_push_back_detail
    {
        template<typename Cont, typename T>
        using push_back_t = decltype(static_cast<void>(
            unwrap_reference(std::declval<Cont &>()).push_back(std::declval<T>())));

        template<typename Cont, typename Rng>
        using insert_t = decltype(static_cast<void>(
            ranges::insert(std::declval<Cont &>(), std::declval<sentinel_t<Cont>>(),
                           std::declval<Rng>())));

        template<typename Cont, typename T>
        auto push_back(Cont && cont, T && t) -> CPP_ret(push_back_t<Cont, T>)( //
            requires LvalueContainerLike<Cont> &&
            (!Range<T>)&&Constructible<range_value_t<Cont>, T>)
        {
            unwrap_reference(cont).push_back(static_cast<T &&>(t));
        }

        template<typename Cont, typename Rng>
        auto push_back(Cont && cont, Rng && rng) -> CPP_ret(insert_t<Cont, Rng>)( //
            requires LvalueContainerLike<Cont> && Range<Rng>)
        {
            ranges::insert(cont, end(cont), static_cast<Rng &&>(rng));
        }

        // clang-format off
        CPP_def
        (
            template(typename Rng, typename T)
            concept PushBackActionConcept,
                requires (Rng &&rng, T &&t)
                (
                    push_back(rng, (T &&) t)
                ) &&
                InputRange<Rng> &&
                    (Range<T> || Constructible<range_value_t<Rng>, T>)
        );
        // clang-format on

        struct push_back_fn
        {
        private:
            friend action::action_access;
            template<typename T>
            static auto bind(push_back_fn push_back, T && val)
            {
                return bind_back(push_back, static_cast<T &&>(val));
            }
            template<typename T, std::size_t N>
            static auto bind(push_back_fn push_back, T (&val)[N])
            {
                return bind_back(
                    [push_back](auto && rng, T(*val)[N])
                        -> invoke_result_t<push_back_fn, decltype(rng), T(&)[N]> {
                        return push_back(static_cast<decltype(rng)>(rng), *val);
                    },
                    &val);
            }

        public:
            template<typename Rng, typename T>
            auto operator()(Rng && rng, T && t) const -> CPP_ret(Rng)( //
                requires PushBackActionConcept<Rng, T>)
            {
                push_back(rng, static_cast<T &&>(t));
                return static_cast<Rng &&>(rng);
            }
        };
    } // namespace adl_push_back_detail
    /// \endcond

    namespace action
    {
        /// \ingroup group-actions
        RANGES_INLINE_VARIABLE(
            detail::with_braced_init_args<action<adl_push_back_detail::push_back_fn>>,
            push_back)
    } // namespace action

    using action::push_back;
} // namespace ranges

#endif
