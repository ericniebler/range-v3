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

        struct SimpleView
        {
            template<typename Rng>
            auto requires_() -> decltype(
                concepts::valid_expr(
                    concepts::model_of<concepts::View, Rng>() &&
                    concepts::model_of<concepts::Range, Rng const>() &&
                    concepts::model_of<concepts::Same, iterator_t<Rng>, iterator_t<Rng const>>() &&
                    concepts::model_of<concepts::Same, sentinel_t<Rng>, sentinel_t<Rng const>>()
                ));
        };

        template<typename Rng>
        constexpr bool simple_view()
        {
            return concepts::models<SimpleView, Rng>::value;
        }

        template<typename Rng>
        using ViewableRange = meta::and_<
            Range<Rng>,
            meta::or_<std::is_lvalue_reference<Rng>, View<uncvref_t<Rng>>>>;

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
                    static auto bind(Ts &&...ts)
                    RANGES_DECLTYPE_AUTO_RETURN
                    (
                        V::bind(static_cast<Ts&&>(ts)...)
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

            template<typename View>
            struct view : pipeable<view<View>>
            {
            private:
                View view_;
                friend pipeable_access;

                template<typename Rng, typename ...Rest>
                using ViewConcept = meta::and_<ViewableRange<Rng>, Invocable<View&, Rng, Rest...>>;

                // Piping requires range arguments or lvalue containers.
                template<typename Rng, typename Vw,
                    CONCEPT_REQUIRES_(ViewConcept<Rng>())>
                static auto pipe(Rng && rng, Vw && v)
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    v.view_(static_cast<Rng&&>(rng))
                )

            #ifndef RANGES_DOXYGEN_INVOKED
                // For better error messages:
                template<typename Rng, typename Vw,
                    CONCEPT_REQUIRES_(!ViewConcept<Rng>())>
                static void pipe(Rng &&, Vw &&)
                {
                    CONCEPT_ASSERT_MSG(Range<Rng>(),
                        "The type Rng must be a model of the Range concept.");
                    // BUGBUG This isn't a very helpful message. This is probably the wrong place
                    // to put this check:
                    CONCEPT_ASSERT_MSG(Invocable<View&, Rng>(),
                        "This view is not callable with this range type.");
                    static_assert(ranges::View<Rng>() || std::is_lvalue_reference<Rng>(),
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
                template<typename Rng, typename...Rest,
                    CONCEPT_REQUIRES_(ViewConcept<Rng, Rest...>())>
                auto operator()(Rng && rng, Rest &&... rest) const
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    view_(static_cast<Rng&&>(rng), static_cast<Rest&&>(rest)...)
                )

                // Currying overload.
                template<typename...Ts, typename V = View>
                auto operator()(Ts &&... ts) const
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    make_view(view_access::impl<V>::bind(view_,
                        static_cast<Ts&&>(ts)...))
                )
            };
            /// \endcond
        }
    }
}

#endif
