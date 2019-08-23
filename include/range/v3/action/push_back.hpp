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
            requires lvalue_container_like<Cont> &&
            (!range<T>)&&constructible_from<range_value_t<Cont>, T>)
        {
            unwrap_reference(cont).push_back(static_cast<T &&>(t));
        }

        template<typename Cont, typename Rng>
        auto push_back(Cont && cont, Rng && rng) -> CPP_ret(insert_t<Cont, Rng>)( //
            requires lvalue_container_like<Cont> && range<Rng>)
        {
            ranges::insert(cont, end(cont), static_cast<Rng &&>(rng));
        }

        /// \cond
        // clang-format off
        CPP_def
        (
            template(typename Rng, typename T)
            concept can_push_back_,
                requires (Rng &&rng, T &&t)
                (
                    push_back(rng, (T &&) t)
                )
        );
        // clang-format on
        /// \endcond

        struct push_back_fn
        {
        private:
            friend actions::action_access;
            template<typename T>
            static auto bind(push_back_fn push_back, T && val)
            {
                return bind_back(push_back, static_cast<T &&>(val));
            }
#ifdef RANGES_WORKAROUND_MSVC_OLD_LAMBDA
            template<typename T, std::size_t N>
            struct lamduh
            {
                T (&val_)[N];

                template<typename Rng>
                auto operator()(Rng && rng) const
                    -> invoke_result_t<push_back_fn, Rng, T (&)[N]>
                {
                    return push_back_fn{}(static_cast<Rng &&>(rng), val_);
                }
            };
            template<typename T, std::size_t N>
            static lamduh<T, N> bind(push_back_fn, T (&val)[N])
            {
                return {val};
            }
#else  // ^^^ workaround / no workaround vvv
            template<typename T, std::size_t N>
            static auto bind(push_back_fn, T (&val)[N])
            {
                return [&val](auto && rng)
                           -> invoke_result_t<push_back_fn, decltype(rng), T(&)[N]>
                {
                    return push_back_fn{}(static_cast<decltype(rng)>(rng), val);
                };
            }
#endif // RANGES_WORKAROUND_MSVC_OLD_LAMBDA
        public:
            template<typename Rng, typename T>
            auto operator()(Rng && rng, T && t) const -> CPP_ret(Rng)( //
                requires input_range<Rng> && can_push_back_<Rng, T> &&
                (range<T> || constructible_from<range_value_t<Rng>, T>))
            {
                push_back(rng, static_cast<T &&>(t));
                return static_cast<Rng &&>(rng);
            }
        };
    } // namespace adl_push_back_detail
    /// \endcond

    namespace actions
    {
        /// \ingroup group-actions
        RANGES_INLINE_VARIABLE(
            detail::with_braced_init_args<action<adl_push_back_detail::push_back_fn>>,
            push_back)
    } // namespace actions

    using actions::push_back;
} // namespace ranges

#endif
