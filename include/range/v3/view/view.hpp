// Range v3 library
//
//  Copyright Eric Niebler 2014
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
#include <range/v3/range_fwd.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/pipeable.hpp>
#include <range/v3/utility/functional.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace detail
        {
            struct null_pipe
            {
                template<typename Rng>
                void operator()(Rng &&) const
                {}
            };
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
                    static auto bind(Ts &&...ts)
                    RANGES_DECLTYPE_AUTO_RETURN
                    (
                        V::bind(std::forward<Ts>(ts)...)
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
            constexpr make_view_fn make_view{};

            template<typename Rng>
            using ViewableIterable = meta::and_<
                Iterable<Rng>,
                meta::or_<std::is_lvalue_reference<Rng>, Range<Rng>>>;

            template<typename View>
            struct view : pipeable<view<View>>
            {
            private:
                View view_;
                friend pipeable_access;

                template<typename Rng, typename ...Rest>
                using ViewConcept = meta::and_<ViewableIterable<Rng>, Function<View, Rng, Rest...>>;

                // Pipeing requires range arguments or lvalue containers.
                template<typename Rng, typename Vw,
                    CONCEPT_REQUIRES_(ViewConcept<Rng>())>
                static auto pipe(Rng && rng, Vw && v)
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    v.view_(std::forward<Rng>(rng))
                )
            #ifndef RANGES_DOXYGEN_INVOKED
                // For better error messages:
                template<typename Rng, typename Vw,
                    CONCEPT_REQUIRES_(!ViewConcept<Rng>())>
                static void pipe(Rng &&, Vw &&)
                {
                    CONCEPT_ASSERT_MSG(Iterable<Rng>(),
                        "The type Rng must be a model of the Iterable concept.");
                    // BUGBUG This isn't a very helpful message. This is probably the wrong place
                    // to put this check:
                    CONCEPT_ASSERT_MSG(Function<View, Rng>(),
                        "This view is not callable with this range type.");
                    static_assert(Range<Rng>() || std::is_lvalue_reference<Rng>(),
                        "You can't pipe an rvalue container into an view. First, save the container into "
                        "a named variable, and then pipe it to the view.");
                }
            #endif
            public:
                view() = default;
                view(View a)
                  : view_(std::move(a))
                {}
                // Calling directly requires range arguments or lvalue containers.
                template<typename Rng, typename...Rest,
                    CONCEPT_REQUIRES_(ViewConcept<Rng, Rest...>())>
                auto operator()(Rng && rng, Rest &&... rest) const
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    view_(std::forward<Rng>(rng), std::forward<Rest>(rest)...)
                )
                // Currying overload.
                template<typename T, typename...Rest, typename V = View>
                auto operator()(T && t, Rest &&... rest) const
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    make_view(view_access::impl<V>::bind(view_, std::forward<T>(t),
                        std::forward<Rest>(rest)...))
                )
            };
            /// \endcond
        }
    }
}

#endif
