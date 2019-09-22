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
    constexpr bool simple_view() noexcept
    {
        return (bool)simple_view_<Rng>;
    }

    struct make_view_closure_fn
    {
        template<typename Fun>
        constexpr views::view_closure<Fun> operator()(Fun fun) const
        {
            return views::view_closure<Fun>{static_cast<Fun &&>(fun)};
        }
    };

    /// \ingroup group-views
    /// \sa make_view_fn
    RANGES_INLINE_VARIABLE(make_view_closure_fn, make_view_closure)

    namespace views
    {
        struct view_closure_base
        {
        private:
            template<typename View1, typename View2>
            struct composed_
            {
                View1 vw1_;
                View2 vw2_;
                // clang-format off
                template<typename Arg>
                constexpr auto CPP_auto_fun(operator())(Arg && arg) (const)
                (
                    return static_cast<Arg &&>(arg) | vw1_ | vw2_
                )
                // clang-format on
            };

        public:
            // Piping requires viewable_ranges.
            CPP_template(typename Rng, typename ViewFn)(                              //
                requires defer::viewable_range<Rng> && defer::invocable<ViewFn, Rng>) //
                friend constexpr auto
                operator|(Rng && rng, view_closure<ViewFn> vw)
            {
                return static_cast<ViewFn &&>(vw)(static_cast<Rng &&>(rng));
            }

            // This overload is deleted because when piping a range into an
            // view, it must be moved in.
            template<typename Rng, typename ViewFn>         // **************************
            friend constexpr auto                           // **************************
            operator|(Rng &&, view_closure<ViewFn> const &) // ******* READ THIS ********
                                                            // **** IF YOUR COMPILE *****
                -> CPP_ret(Rng)(                            // ****** BREAKS HERE *******
                    requires range<Rng> &&                  // **************************
                    (!viewable_range<Rng>)) = delete;       // **************************
            // **************************************************************************
            // *    When piping a range into an adaptor, the range must satisfy the     *
            // *    "viewable_range" concept. A range is viewable when either or both   *
            // *    of these things are true:                                           *
            // *      - The range is an lvalue (not a temporary object), OR             *
            // *      - The range is a view (not a container).                          *
            // **************************************************************************

            template<typename View1, typename View2>
            friend constexpr auto operator|(view_closure<View1> vw1,
                                            view_closure<View2> vw2)
            {
                return make_view_closure(
                    composed_<view_closure<View1>, view_closure<View2>>{
                        static_cast<view_closure<View1> &&>(vw1),
                        static_cast<view_closure<View2> &&>(vw2)});
            }
        };

        template<typename ViewFn>
        struct RANGES_EMPTY_BASES view_closure
          : view_closure_base
          , ViewFn
        {
            view_closure() = default;

            constexpr explicit view_closure(ViewFn fn)
              : ViewFn(static_cast<ViewFn &&>(fn))
            {}

            using ViewFn::operator();
        };

        /// \cond
        /// DEPRECATED STUFF
        struct view_access_
        {
            template<typename ViewFn>
            struct impl
            {
                // clang-format off
                template<typename... Ts, typename V = ViewFn>
                static constexpr auto CPP_auto_fun(bind)(Ts &&... ts)
                (
                    return V::bind(static_cast<Ts &&>(ts)...)
                )
                // clang-format on
            };
        };

        using view_access RANGES_DEPRECATED(
            "view_access and views::view<> are deprecated. Please "
            "replace view<> with view_closure<> and discontinue use of view_access.") =
            view_access_;

        template<typename>
        struct view;

        struct make_view_fn_
        {
            template<typename Fun>
            constexpr view<Fun> operator()(Fun fun) const
            {
                return view<Fun>{static_cast<Fun &&>(fun)};
            }
        };
        using make_view_fn RANGES_DEPRECATED(
            "make_view_fn is deprecated. Please use "
            "make_view_closure instead.") = make_view_fn_;

        namespace
        {
            RANGES_DEPRECATED(
                "make_view and views::view<> has been deprecated. Please switch to "
                "make_view_closure and views::view_closure.")
            RANGES_INLINE_VAR constexpr auto & make_view =
                static_const<make_view_fn_>::value;
        } // namespace

        template<typename View>
        struct RANGES_DEPRECATED(
            "The views::view<> template is deprecated. Please switch to view_closure")
            view : pipeable_base
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
                return make_view_fn_{}(
                    view_access::impl<V>::bind(view_, static_cast<Ts &&>(ts)...))
            )
            // clang-format on
        };
        /// \endcond
    } // namespace views

    template<typename ViewFn>
    RANGES_INLINE_VAR constexpr bool is_pipeable_v<views::view_closure<ViewFn>> = true;
    /// @}
} // namespace ranges

#endif
