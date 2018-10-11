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
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    inline namespace v3
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
        }
        /// \endcond

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
                    static auto CPP_auto_fun(bind)(Ts &&...ts)
                    (
                        return V::bind(static_cast<Ts &&>(ts)...)
                    )
                };
            };

            struct make_view_fn
            {
                template<typename Fun>
                view<Fun> operator()(Fun fun) const
                {
                    return {std::move(fun)};
                }
            };

            /// \ingroup group-views
            /// \sa make_view_fn
            RANGES_INLINE_VARIABLE(make_view_fn, make_view)

            CPP_def
            (
                template(typename Rng)
                concept ViewableRange,
                    Range<Rng> &&
                    (std::is_lvalue_reference<Rng>::value || View<uncvref_t<Rng>>)
            );

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
                CPP_template(typename Rng, typename Vw)(
                    requires ViewConcept<View, Rng>)
                static auto CPP_auto_fun(pipe)(Rng &&rng, Vw &&v)
                (
                    return v.view_(static_cast<Rng &&>(rng))
                )

            #ifndef RANGES_DOXYGEN_INVOKED
                // For better error messages:
                template<typename Rng, typename Vw>
                static auto pipe(Rng &&, Vw &&) ->
                    CPP_ret(void)(
                        requires not ViewConcept<View const, Rng>)
                {
                    CPP_assert_msg(Range<Rng>,
                        "The type Rng must be a model of the Range concept.");
                    // BUGBUG This isn't a very helpful message. This is probably the wrong place
                    // to put this check:
                    CPP_assert_msg(Invocable<View&, Rng>,
                        "This view is not callable with this range type.");
                    static_assert((bool)ranges::View<Rng> || std::is_lvalue_reference<Rng>(),
                        "You can't pipe an rvalue container into a view. First, save the container into "
                        "a named variable, and then pipe it to the view.");
                }
            #endif

            public:
                view() = default;
                view(View a)
                  : view_(std::move(a))
                {}

                // Calling directly requires View arguments or lvalue containers.
                CPP_template(typename Rng, typename...Rest)(
                    requires ViewConcept<View const, Rng, Rest...>)
                auto CPP_auto_fun(operator())(Rng &&rng, Rest &&... rest) (const)
                (
                    return view_(static_cast<Rng &&>(rng), static_cast<Rest &&>(rest)...)
                )

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
}

#endif
