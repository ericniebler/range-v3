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

#ifndef RANGES_V3_VIEW_FOR_EACH_HPP
#define RANGES_V3_VIEW_FOR_EACH_HPP

#include <utility>
#include <range/v3/range_fwd.hpp>
#include <range/v3/view/view.hpp>
#include <range/v3/view/join.hpp>
#include <range/v3/view/generate_n.hpp>
#include <range/v3/view/repeat_n.hpp>
#include <range/v3/view/single.hpp>
#include <range/v3/view/transform.hpp>
#include <range/v3/utility/functional.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-views
        /// @{
        template<typename Rng, typename F>
        struct for_each_view
          : join_view<transform_view<Rng, F>>
        {
            for_each_view() = default;
            for_each_view(Rng && rng, F f)
              : join_view<transform_view<Rng, F>>{{std::forward<Rng>(rng), std::move(f)}}
            {}
        };

        namespace view
        {
            struct for_each_fn
            {
            private:
                friend view_access;
                template<typename F>
                static auto bind(for_each_fn for_each, F f)
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    make_pipeable(std::bind(for_each, std::placeholders::_1, protect(std::move(f))))
                )
            public:
                template<typename Rng, typename F>
                using Concept = meta::and_<
                    Iterable<Rng>,
                    Invokable<F, range_value_t<Rng>>,
                    Iterable<concepts::Invokable::result_t<F, range_value_t<Rng>>>>;

                template<typename Rng, typename F,
                    CONCEPT_REQUIRES_(Concept<Rng, F>())>
                for_each_view<Rng, F> operator()(Rng && rng, F f) const
                {
                    return {std::forward<Rng>(rng), std::move(f)};
                }

            #ifndef RANGES_DOXYGEN_INVOKED
                // For better error reporting
                template<typename Rng, typename F,
                    CONCEPT_REQUIRES_(!Concept<Rng, F>())>
                void operator()(Rng &&, F) const
                {
                    CONCEPT_ASSERT_MSG(Iterable<Rng>(),
                        "Rng is not a model of the Iterable concept.");
                    CONCEPT_ASSERT_MSG(Invokable<F, range_value_t<Rng>>(),
                        "The function F is not callable with arguments of the type of the range's value type.");
                    CONCEPT_ASSERT_MSG(Iterable<concepts::Invokable::result_t<F, range_value_t<Rng>>>(),
                        "To use view::for_each, the function F must return a model of the Iterable concept.");
                }
            #endif
            };

            /// \sa `for_each_fn`
            /// \ingroup group-views
            constexpr view<for_each_fn> for_each{};
        }

        struct yield_fn
        {
            template<typename V>
            single_view<V> operator()(V v) const
            {
                return view::single(std::move(v));
            }
        };

        /// \sa `yield_fn`
        /// \ingroup group-views
        constexpr yield_fn yield{};

        struct yield_from_fn
        {
            template<typename Rng, CONCEPT_REQUIRES_(Range<Rng>())>
            Rng operator()(Rng rng) const
            {
                return rng;
            }
        };

        /// \sa `yield_from_fn`
        /// \ingroup group-views
        constexpr yield_from_fn yield_from{};

        struct yield_if_fn
        {
            template<typename V>
            repeat_n_view<V> operator()(bool b, V v) const
            {
                return view::repeat_n(std::move(v), b ? 1 : 0);
            }
        };

        /// \sa `yield_if_fn`
        /// \ingroup group-views
        constexpr yield_if_fn yield_if{};

        struct lazy_yield_if_fn
        {
            template<typename F>
            generate_n_view<F> operator()(bool b, F f) const
            {
                CONCEPT_ASSERT(Function<F>());
                return view::generate_n(std::move(f), b ? 1 : 0);
            }
        };

        /// \sa `lazy_yield_if_fn`
        /// \ingroup group-views
        constexpr lazy_yield_if_fn lazy_yield_if{};
        /// @}

        /// \cond
        template<typename Rng, typename Fun,
            typename Result = concepts::Function::result_t<Fun, range_value_t<Rng>>,
            CONCEPT_REQUIRES_(Iterable<Rng>() &&
                              Function<Fun, range_value_t<Rng>>() &&
                              Iterable<Result>())>
        auto operator >>= (Rng && rng, Fun fun) ->
            decltype(view::for_each(std::forward<Rng>(rng), std::move(fun)))
        {
            return view::for_each(std::forward<Rng>(rng), std::move(fun));
        }
        /// \endcond
    }
}

#endif
