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

#include <range/v3/functional/compose.hpp>
#include <range/v3/functional/concepts.hpp>
#include <range/v3/functional/pipeable.hpp>
#include <range/v3/functional/reference_wrapper.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/range/traits.hpp>
#include <range/v3/utility/static_const.hpp>

#include <range/v3/detail/disable_warnings.hpp>

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

        struct view_closure_base_
        {};
    } // namespace detail
      /// \endcond

    // clang-format off
    template<typename Rng>
    CPP_concept_fragment(simple_view_frag_, (Rng),
        same_as<iterator_t<Rng>, iterator_t<Rng const>> &&
        same_as<sentinel_t<Rng>, sentinel_t<Rng const>>
    );
    template<typename Rng>
    CPP_concept_bool simple_view_ =
        view_<Rng> &&
        range<Rng const> &&
        CPP_fragment(ranges::simple_view_frag_, Rng);

    template<typename ViewFn, typename Rng>
    CPP_concept_fragment(invocable_view_closure_, (ViewFn, Rng),
        !derived_from<invoke_result_t<ViewFn, Rng>, detail::view_closure_base_>
    );
    template<typename ViewFn, typename Rng>
    CPP_concept_bool invocable_view_closure =
        invocable<ViewFn, Rng> &&
        CPP_fragment(ranges::invocable_view_closure_, ViewFn, Rng);

    namespace defer
    {
        template<typename ViewFn, typename Rng>
        CPP_concept invocable_view_closure =
            CPP_defer(ranges::invocable_view_closure, ViewFn, Rng);
    }
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
    /// \sa make_view_closure_fn
    RANGES_INLINE_VARIABLE(make_view_closure_fn, make_view_closure)

    namespace views
    {
        struct RANGES_STRUCT_WITH_ADL_BARRIER(view_closure_base)
          : detail::view_closure_base_
        {
            // Piping requires viewable_ranges. Pipeing a value into a closure
            // should not yield another closure.
            CPP_template(typename Rng, typename ViewFn)(        //
                requires defer::viewable_range<Rng> &&          //
                    defer::invocable_view_closure<ViewFn, Rng>) //
                friend constexpr auto
                operator|(Rng && rng, view_closure<ViewFn> vw)
            {
                return static_cast<ViewFn &&>(vw)(static_cast<Rng &&>(rng));
            }

#ifndef RANGES_WORKAROUND_CLANG_43400
            // This overload is deleted because when piping a range into an
            // view, it must be moved in.
            template<typename Rng, typename ViewFn>         // **************************
            friend constexpr auto                           // **************************
            operator|(Rng &&, view_closure<ViewFn> const &) // ******* READ THIS ********
                                                            // **** IF YOUR COMPILE *****
                -> CPP_broken_friend_ret(Rng)(              // ****** BREAKS HERE *******
                    requires range<Rng> &&                  // **************************
                    (!viewable_range<Rng>)) = delete;       // **************************
            // **************************************************************************
            // *    When piping a range into an adaptor, the range must satisfy the     *
            // *    "viewable_range" concept. A range is viewable when either or both   *
            // *    of these things are true:                                           *
            // *      - The range is an lvalue (not a temporary object), OR             *
            // *      - The range is a view (not a container).                          *
            // **************************************************************************
#endif

            template<typename ViewFn, typename Pipeable>
            friend constexpr auto operator|(view_closure<ViewFn> vw, Pipeable pipe)
                -> CPP_broken_friend_ret(view_closure<composed<Pipeable, ViewFn>>)(
                    requires(is_pipeable_v<Pipeable>))
            {
                return make_view_closure(
                    compose(static_cast<Pipeable &&>(pipe), static_cast<ViewFn &&>(vw)));
            }
        };

#ifdef RANGES_WORKAROUND_CLANG_43400
        namespace RANGES_ADL_BARRIER_FOR(view_closure_base)
        {
            // This overload is deleted because when piping a range into an
            // view, it must be moved in.
            template<typename Rng, typename ViewFn>         // **************************
            constexpr auto                                  // **************************
            operator|(Rng &&, view_closure<ViewFn> const &) // ******* READ THIS ********
                                                            // **** IF YOUR COMPILE *****
                ->CPP_ret(Rng)(                             // ****** BREAKS HERE *******
                    requires range<Rng> &&                  // **************************
                    (!viewable_range<Rng>)) = delete;       // **************************
            // **************************************************************************
            // *    When piping a range into an adaptor, the range must satisfy the     *
            // *    "viewable_range" concept. A range is viewable when either or both   *
            // *    of these things are true:                                           *
            // *      - The range is an lvalue (not a temporary object), OR             *
            // *      - The range is a view (not a container).                          *
            // **************************************************************************
        } // namespace )
#endif    // RANGES_WORKAROUND_CLANG_43400

        template<typename ViewFn>
        struct RANGES_EMPTY_BASES view_closure
          : view_closure_base
          , ViewFn
        {
            view_closure() = default;

            constexpr explicit view_closure(ViewFn fn)
              : ViewFn(static_cast<ViewFn &&>(fn))
            {}
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
        struct old_view_;

        struct make_view_fn_
        {
            template<typename Fun>
            constexpr old_view_<Fun> operator()(Fun fun) const
            {
                return old_view_<Fun>{static_cast<Fun &&>(fun)};
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

        template<typename ViewFn>
        struct old_view_ : pipeable_base
        {
        private:
            ViewFn vw_;
            friend pipeable_access;

            // Piping requires range arguments or lvalue containers.
            template<typename Rng, typename Vw>
            static constexpr auto CPP_fun(pipe)(Rng && rng, Vw && v)( //
                requires viewable_range<Rng> && invocable<ViewFn &, Rng>)
            {
                return v.vw_(static_cast<Rng &&>(rng));
            }

        public:
            old_view_() = default;

            constexpr explicit old_view_(ViewFn a) noexcept(
                std::is_nothrow_move_constructible<ViewFn>::value)
              : vw_(std::move(a))
            {}

            // Calling directly requires a viewable_range.
            template<typename Rng, typename... Rest>
            constexpr auto operator()(Rng && rng, Rest &&... rest) const -> CPP_ret(
                invoke_result_t<ViewFn const &, Rng, Rest...>)( //
                requires viewable_range<Rng> && invocable<ViewFn const &, Rng, Rest...>)
            {
                return vw_(static_cast<Rng &&>(rng), static_cast<Rest &&>(rest)...);
            }

            // Currying overload.
            // clang-format off
            template<typename... Ts, typename V = ViewFn>
            constexpr auto CPP_auto_fun(operator())(Ts &&... ts)(const)
            (
                return make_view_fn_{}(
                    view_access_::impl<V>::bind(vw_, static_cast<Ts &&>(ts)...))
            )
            // clang-format on
        };

        template<typename ViewFn>
        using view RANGES_DEPRECATED(
            "The views::view<> template is deprecated. Please switch to view_closure") =
            old_view_<ViewFn>;
        /// \endcond
    } // namespace views

    template<typename ViewFn>
    RANGES_INLINE_VAR constexpr bool is_pipeable_v<views::view_closure<ViewFn>> = true;
    /// @}
} // namespace ranges

#include <range/v3/detail/reenable_warnings.hpp>

#endif
