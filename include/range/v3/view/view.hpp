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

    namespace views
    {
        /// \addtogroup group-views
        /// @{
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

        struct make_view_fn
        {
            template<typename Fun>
            constexpr view<Fun> operator()(Fun fun) const
            {
                return view<Fun>{std::move(fun)};
            }
        };

        /// \ingroup group-views
        /// \sa make_view_fn
        RANGES_INLINE_VARIABLE(make_view_fn, make_view)

        template<typename View>
        struct view : pipeable_base
        {
        private:
            View view_;
            friend pipeable_access;

            // Piping requires range arguments or lvalue containers.
            template<typename Rng, typename Vw>
            static constexpr auto CPP_fun(pipe)(Rng && rng, Vw && v)( //
                requires viewable_range<Rng> && invocable<View &, Rng>)
            {
                return v.view_(static_cast<Rng &&>(rng));
            }

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
            template<typename... Ts, typename V = View>
            constexpr auto CPP_auto_fun(operator())(Ts &&... ts)(const)
            (
                return make_view(
                    view_access::impl<V>::bind(view_, static_cast<Ts &&>(ts)...))
            )
            // clang-format on
        };
        /// \endcond
    } // namespace views
} // namespace ranges

#endif
