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

#include <utility>
#include <type_traits>
#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/range/traits.hpp>
#include <range/v3/functional/concepts.hpp>
#include <range/v3/functional/pipeable.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    /// \cond
    namespace detail
    {
        struct null_pipe
        {
            template<typename Rng>
            void operator()(Rng &&) const
            {}
        };

        struct dereference_fn
        {
            template<typename I>
            constexpr auto CPP_auto_fun(operator())(I &&i) (const)
            (
                return *(I &&) i
            )
        };
    }
    /// \endcond

    CPP_def
    (
        template(typename Rng)
        concept SimpleView,
            View<Rng> &&
            Range<Rng const> &&
            Same<iterator_t<Rng>, iterator_t<Rng const>> &&
            Same<sentinel_t<Rng>, sentinel_t<Rng const>>
    );

    template<typename Rng>
    constexpr bool simple_view()
    {
        return (bool) SimpleView<Rng>;
    }

    namespace view
    {
        /// \addtogroup group-views
        /// @{
        struct view_access
        {
            template<typename View>
            struct impl
            {
                template<typename...Ts, typename V = View>
                static constexpr auto CPP_auto_fun(bind)(Ts &&...ts)
                (
                    return V::bind(static_cast<Ts &&>(ts)...)
                )
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

        CPP_def
        (
            template(typename View, typename Rng, typename ...Rest)
            (concept ViewConcept)(View, Rng, Rest...),
                ViewableRange<Rng> &&
                Invocable<View&, Rng, Rest...>
        );

        template<typename View>
        struct view : pipeable<view<View>>
        {
        private:
            View view_;
            friend pipeable_access;

            // Piping requires range arguments or lvalue containers.
            template<typename Rng, typename Vw>
            static auto CPP_fun(pipe)(Rng &&rng, Vw &&v)(
                requires ViewableRange<Rng> && Invocable<View &, Rng>)
            {
                return v.view_(static_cast<Rng &&>(rng));
            }
        public:
            view() = default;

            constexpr explicit view(View a)
                noexcept(std::is_nothrow_move_constructible<View>::value)
              : view_(std::move(a))
            {}

            // Calling directly requires a ViewableRange.
            template<typename Rng, typename...Rest>
            auto operator()(Rng &&rng, Rest &&... rest) const ->
                CPP_ret(invoke_result_t<View const &, Rng, Rest...>)(
                    requires ViewableRange<Rng> && Invocable<View const &, Rng, Rest...>)
            {
                return view_(static_cast<Rng &&>(rng), static_cast<Rest &&>(rest)...);
            }

            // Currying overload.
            template<typename...Ts, typename V = View>
            auto CPP_auto_fun(operator())(Ts &&... ts) (const)
            (
                return make_view(view_access::impl<V>::bind(view_,
                    static_cast<Ts &&>(ts)...))
            )
        };
        /// \endcond
    }
}

#endif
