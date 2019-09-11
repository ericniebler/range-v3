/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2014-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGES_V3_VIEW_VIEW_HPP
#define RANGES_V3_VIEW_VIEW_HPP

#include <type_traits>
#include <utility>

#include <meta/meta.hpp>

#include <range/v3/range_fwd.hpp>

#include <range/v3/functional/concepts.hpp>
#include <range/v3/functional/pipeable.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/range/traits.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    /// \addtogroup group-views
    /// @{

    /// \cond
    namespace detail
    {
        struct null_pipe
        {
            template<typename Rng>
            constexpr void operator()(Rng &&) const
            {}
        };

        struct dereference_fn
        {
            // clang-format off
            template<typename I>
            constexpr auto CPP_auto_fun(operator())(I &&i) (const)
            (
                return *(I &&) i
            )
            // clang-format on
        };
    } // namespace detail
      /// \endcond

    // clang-format off
    CPP_def
    (
        template(typename Rng)
        concept simple_view_,
            view_<Rng> &&
            range<Rng const> &&
            same_as<iterator_t<Rng>, iterator_t<Rng const>> &&
            same_as<sentinel_t<Rng>, sentinel_t<Rng const>>
    );
    // clang-format on

    template<typename Rng>
    constexpr bool simple_view()
    {
        return (bool)simple_view_<Rng>;
    }

    struct make_view_fn
    {
        template<typename Fun>
        constexpr views::view<Fun> operator()(Fun fun) const
        {
            return views::view<Fun>{std::move(fun)};
        }
    };

    /// \ingroup group-views
    /// \sa make_view_fn
    RANGES_INLINE_VARIABLE(make_view_fn, make_view)

    namespace views
    {
        struct view_access
        {
            template<typename View>
            struct impl
            {
                // clang-format off
                template<typename... Ts, typename V = View>
                static constexpr auto CPP_auto_fun(bind)(Ts &&... ts)
                (
                    return V::bind(static_cast<Ts &&>(ts)...)
                )
                // clang-format on
            };
        };

        using ranges::make_view; // for legacy reasons

        struct view_base
        {
        private:
            // clang-format off
            template<typename View1, typename View2>
            struct composed
            {
                View1 vw1_;
                View2 vw2_;
                template<typename Arg>
                constexpr auto CPP_auto_fun(operator())(Arg && arg) (const)
                (
                    return static_cast<Arg &&>(arg) | vw1_ | vw2_
                )
            };
            // clang-format on
        public:
            template<typename View>
            constexpr static View && get_view(view<View> && vw) noexcept
            {
                return static_cast<View &&>(vw.view_);
            }

            // Piping requires viewable_ranges.
            CPP_template(typename Rng, typename View)(                              //
                requires defer::viewable_range<Rng> && defer::invocable<View, Rng>) //
                constexpr friend auto
                operator|(Rng && rng, view<View> vw)
            {
                return get_view(static_cast<view<View> &&>(vw))(static_cast<Rng &&>(rng));
            }

            // This overload is deleted because when piping a range into an
            // view, it must be moved in.
            template<typename Rng, typename View>
            // RANGES_DEPRECATED("You must pipe a viewable_range (e.g. an lvalue"
            // " container or another view) into an view.")
            constexpr friend auto operator|(Rng &&, view<View> const &)
                -> CPP_ret(Rng)(requires range<Rng> && (!viewable_range<Rng>)) = delete;

            template<typename View1, typename View2>
            constexpr friend auto operator|(view<View1> vw1, view<View2> vw2)
            {
                return make_view(composed<view<View1>, view<View2>>{
                    static_cast<view<View1> &&>(vw1), static_cast<view<View2> &&>(vw2)});
            }
        };

        template<typename View>
        struct view : view_base
        {
        private:
            View view_;
            friend view_base;

        public:
            view() = default;

            constexpr explicit view(View a) noexcept(
                std::is_nothrow_move_constructible<View>::value)
              : view_(std::move(a))
            {}

            // Calling directly requires a viewable_range.
            template<typename Rng, typename... Rest>
            constexpr auto operator()(Rng && rng, Rest &&... rest) const
                -> CPP_ret(invoke_result_t<View const &, Rng, Rest...>)( //
                    requires viewable_range<Rng> && invocable<View const &, Rng, Rest...>)
            {
                return view_(static_cast<Rng &&>(rng), static_cast<Rest &&>(rest)...);
            }

            // Currying overload.
            // clang-format off
            CPP_template(typename... Rest, typename V = View)(
                requires(sizeof...(Rest) != 0))
            constexpr auto CPP_auto_fun(operator())(Rest &&... rest)(const)
            (
                return make_view(
                    view_access::impl<V>::bind(view_,
                                               static_cast<Rest &&>(rest)...))
            )
            // clang-format on
        };
        /// \endcond
    } // namespace views

    template<typename View>
    RANGES_INLINE_VAR constexpr bool is_pipeable_v<views::view<View>> = true;
    /// @}
} // namespace ranges

#endif
