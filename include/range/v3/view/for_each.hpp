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

#ifndef RANGES_V3_VIEW_FOR_EACH_HPP
#define RANGES_V3_VIEW_FOR_EACH_HPP

#include <utility>
#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/detail/satisfy_boost_range.hpp>
#include <range/v3/view/view.hpp>
#include <range/v3/view/all.hpp>
#include <range/v3/view/join.hpp>
#include <range/v3/view/generate_n.hpp>
#include <range/v3/view/repeat_n.hpp>
#include <range/v3/view/single.hpp>
#include <range/v3/view/transform.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace view
        {
            /// Lazily applies an unary function to each element in the source
            /// range that returns another range (possibly empty), flattening
            /// the result.
            struct for_each_fn
            {
            private:
                friend view_access;
                template<typename Fun>
                static auto bind(for_each_fn for_each, Fun fun)
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    make_pipeable(std::bind(for_each, std::placeholders::_1, protect(std::move(fun))))
                )

                struct Concept_
                {
                    template<typename Rng, typename Fun>
                    auto requires_() -> decltype(
                        concepts::valid_expr(
                            concepts::is_true(transform_fn::Concept<Rng, Fun>()),
                            concepts::model_of<
                                concepts::InputRange,
                                concepts::Invocable::result_t<Fun&, range_reference_t<Rng>>>()));
                };

            public:
                template<typename Rng, typename Fun>
                using Concept = concepts::models<Concept_, Rng, Fun>;

                template<typename Rng, typename Fun,
                    CONCEPT_REQUIRES_(Concept<Rng, Fun>())>
                auto operator()(Rng && rng, Fun fun) const
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    join(transform(static_cast<Rng&&>(rng), std::move(fun)))
                )

        #ifndef RANGES_DOXYGEN_INVOKED
                template<typename Rng, typename Fun,
                    CONCEPT_REQUIRES_(!Concept<Rng, Fun>())>
                void operator()(Rng &&, Fun) const
                {
                    CONCEPT_ASSERT_MSG(InputRange<Rng>(),
                        "The object on which view::for_each operates must be a model of the "
                        "InputRange concept.");
                    CONCEPT_ASSERT_MSG(
                        CopyConstructible<Fun>(),
                        "The function passed to view::for_each must be CopyConstructible.");
                    CONCEPT_ASSERT_MSG(
                        Invocable<Fun&, range_reference_t<Rng>>(),
                        "The function passed to view::for_each must be callable with an argument "
                        "of the range's reference type.");
                    CONCEPT_ASSERT_MSG(InputRange<concepts::Invocable::result_t<
                        Fun&, range_reference_t<Rng>>>(),
                        "To use view::for_each, the function F must return a model of the InputRange "
                        "concept.");
                }
        #endif
            };

            /// \relates for_each_fn
            /// \ingroup group-views
            RANGES_INLINE_VARIABLE(view<for_each_fn>, for_each)
        }

        struct yield_fn
        {
            template<typename V,
                CONCEPT_REQUIRES_(CopyConstructible<V>())>
            single_view<V> operator()(V v) const
            {
                return view::single(std::move(v));
            }

        #ifndef RANGES_DOXYGEN_INVOKED
            template<typename Arg, typename Val = detail::decay_t<Arg>,
                CONCEPT_REQUIRES_(!(CopyConstructible<Val>() && Constructible<Val, Arg>()))>
            void operator()(Arg &&) const
            {
                CONCEPT_ASSERT_MSG(CopyConstructible<Val>(),
                    "The object passed to yield must be a model of the CopyConstructible "
                    "concept; that is, it needs to be copy and move constructible, and destructible.");
                CONCEPT_ASSERT_MSG(!CopyConstructible<Val>() || Constructible<Val, Arg>(),
                    "The object type passed to yield must be initializable from the "
                    "actual argument expression.");
            }
        #endif
        };

        /// \relates yield_fn
        /// \ingroup group-views
        RANGES_INLINE_VARIABLE(yield_fn, yield)

        struct yield_from_fn
        {
            template<typename Rng, CONCEPT_REQUIRES_(View<Rng>())>
            Rng operator()(Rng rng) const
            {
                return rng;
            }
        };

        /// \relates yield_from_fn
        /// \ingroup group-views
        RANGES_INLINE_VARIABLE(yield_from_fn, yield_from)

        struct yield_if_fn
        {
            template<typename V>
            repeat_n_view<V> operator()(bool b, V v) const
            {
                return view::repeat_n(std::move(v), b ? 1 : 0);
            }
        };

        /// \relates yield_if_fn
        /// \ingroup group-views
        RANGES_INLINE_VARIABLE(yield_if_fn, yield_if)

        struct lazy_yield_if_fn
        {
            template<typename F>
            generate_n_view<F> operator()(bool b, F f) const
            {
                CONCEPT_ASSERT(Invocable<F&>());
                return view::generate_n(std::move(f), b ? 1 : 0);
            }
        };

        /// \relates lazy_yield_if_fn
        /// \ingroup group-views
        RANGES_INLINE_VARIABLE(lazy_yield_if_fn, lazy_yield_if)
        /// @}

        /// \cond
        template<typename Rng, typename Fun,
            CONCEPT_REQUIRES_(Range<Rng>() && CopyConstructible<Fun>() &&
                Invocable<Fun&, range_common_reference_t<Rng>>() &&
                Range<invoke_result_t<Fun&, range_common_reference_t<Rng>>>())>
        auto operator >>= (Rng && rng, Fun fun) ->
            decltype(view::for_each(static_cast<Rng&&>(rng), std::move(fun)))
        {
            return view::for_each(static_cast<Rng&&>(rng), std::move(fun));
        }
        /// \endcond
    }
}

#endif
