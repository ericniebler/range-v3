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

#ifndef RANGES_V3_VIEW_MAP_HPP
#define RANGES_V3_VIEW_MAP_HPP

#include <utility>
#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <concepts/concepts.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/view/view.hpp>
#include <range/v3/view/transform.hpp>

// TODO: Reuse subrange's PairLike concept here and have get_first and get_second
// dispatch through get<>()

namespace ranges
{
    /// \cond
    namespace detail
    {
        template<typename T>
        constexpr /*c++14*/ T&
        get_first_second_helper(T& t, std::true_type) noexcept
        {
            return t;
        }

        template<typename T>
        constexpr /*c++14*/
        auto get_first_second_helper(T& t, std::false_type)
            noexcept(std::is_nothrow_move_constructible<T>::value) ->
            CPP_ret(T)(
                requires MoveConstructible<T>)
        {
            return std::move(t);
        }

        template<typename P, typename E>
        using get_first_second_tag = meta::bool_<
            std::is_lvalue_reference<P>::value ||
            std::is_lvalue_reference<E>::value>;

        struct get_first
        {
            template<typename Pair>
            constexpr /*c++14*/ auto CPP_auto_fun(operator())(Pair &&p) (const)
            (
                return get_first_second_helper(p.first,
                    get_first_second_tag<Pair, decltype(p.first)>{})
            )
        };

        struct get_second
        {
            template<typename Pair>
            constexpr /*c++14*/ auto CPP_auto_fun(operator())(Pair &&p) (const)
            (
                return get_first_second_helper(p.second,
                    get_first_second_tag<Pair, decltype(p.second)>{})
            )
        };

        CPP_def
        (
            template(typename T)
            concept PairLike,
                Invocable<get_first const &, T> &&
                Invocable<get_second const &, T>
        );
    }
    /// \endcond

    /// \addtogroup group-views
    /// @{
    namespace view
    {
        struct keys_fn
        {
            template<typename Rng>
            auto operator()(Rng &&rng) const ->
                CPP_ret(keys_range_view<all_t<Rng>>)(
                    requires ViewableRange<Rng> && InputRange<Rng> &&
                        detail::PairLike<range_reference_t<Rng>>)
            {
                return {all(static_cast<Rng &&>(rng)), detail::get_first{}};
            }
        };

        struct values_fn
        {
            template<typename Rng>
            auto operator()(Rng &&rng) const ->
                CPP_ret(values_view<all_t<Rng>>)(
                    requires ViewableRange<Rng> && InputRange<Rng> &&
                        detail::PairLike<range_reference_t<Rng>>)
            {
                return {all(static_cast<Rng &&>(rng)), detail::get_second{}};
            }
        };

        /// \relates keys_fn
        /// \ingroup group-views
        RANGES_INLINE_VARIABLE(view<keys_fn>, keys)

        /// \relates values_fn
        /// \ingroup group-views
        RANGES_INLINE_VARIABLE(view<values_fn>, values)
    }
    /// @}
}

#endif
