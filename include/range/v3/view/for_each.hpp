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
#include <range/v3/view/flatten.hpp>
#include <range/v3/view/generate_n.hpp>
#include <range/v3/view/repeat_n.hpp>
#include <range/v3/view/single.hpp>
#include <range/v3/view/transform.hpp>
#include <range/v3/utility/functional.hpp>

namespace ranges
{
    inline namespace v3
    {
        template<typename Rng, typename F>
        struct for_each_view
          : flatten_view<transformed_view<Rng, F>>
        {
            for_each_view() = default;
            for_each_view(Rng && rng, F f)
              : flatten_view<transformed_view<Rng, F>>{{std::forward<Rng>(rng), std::move(f)}}
            {}
        };

        namespace view
        {
            struct for_each_fn
            {
                template<typename Rng, typename F>
                for_each_view<Rng, F> operator()(Rng && rng, F f) const
                {
                    CONCEPT_ASSERT(Iterable<Rng>());
                    CONCEPT_ASSERT(Invokable<F, range_value_t<Rng>>());
                    CONCEPT_ASSERT(Iterable<concepts::Invokable::result_t<F, range_value_t<Rng>>>());
                    return {std::forward<Rng>(rng), std::move(f)};
                }

                template<typename F>
                auto operator()(F f) const ->
                    decltype(make_pipeable(std::bind(*this, std::placeholders::_1, protect(std::move(f)))))
                {
                    return make_pipeable(std::bind(*this, std::placeholders::_1, protect(std::move(f))));
                }
            };

            constexpr for_each_fn for_each {};
        }

        struct yield_fn
        {
            template<typename V>
            single_view<V> operator()(V v) const
            {
                return view::single(std::move(v));
            }
        };

        constexpr yield_fn yield{};

        struct yield_from_fn
        {
            template<typename Rng, CONCEPT_REQUIRES_(Range<Rng>())>
            Rng operator()(Rng rng) const
            {
                return rng;
            }
        };

        constexpr yield_from_fn yield_from{};

        struct yield_if_fn
        {
            template<typename V>
            repeated_n_view<V> operator()(bool b, V v) const
            {
                return view::repeat_n(std::move(v), b ? 1 : 0);
            }
        };

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

        constexpr lazy_yield_if_fn lazy_yield_if{};

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
    }
}

#endif
