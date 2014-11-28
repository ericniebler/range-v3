// Range v3 library
//
//  Copyright Eric Niebler 2013-2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGES_V3_VIEW_REPLACE_IF_HPP
#define RANGES_V3_VIEW_REPLACE_IF_HPP

#include <utility>
#include <type_traits>
#include <range/v3/range_fwd.hpp>
#include <range/v3/view/transform.hpp>
#include <range/v3/utility/pipeable.hpp>
#include <range/v3/utility/concepts.hpp>
#include <range/v3/utility/invokable.hpp>
#include <range/v3/utility/functional.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace view
        {
            struct replace_if_fn
            {
            private:
                template<typename Pred, typename Val>
                struct replacer_if_fun
                {
                private:
                    friend struct replace_if_fn;
                    compressed_pair<semiregular_invokable_t<Pred>, Val> fun_and_new_value_;

                    template<typename Val2>
                    replacer_if_fun(Pred pred, Val2 new_value)
                      : fun_and_new_value_{invokable(std::move(pred)), std::move(new_value)}
                    {}
                public:
                    template<typename Other,
                        CONCEPT_REQUIRES_(!Invokable<Pred const, Other &>())>
                    uncvref_t<Other> operator()(Other && other)
                    {
                        return (fun_and_new_value_.first(other)) ?
                            fun_and_new_value_.second : std::forward<Other>(other);
                    }
                    template<typename Other,
                        CONCEPT_REQUIRES_(Invokable<Pred const, Other &>())>
                    uncvref_t<Other> operator()(Other && other) const
                    {
                        return (fun_and_new_value_.first(other)) ?
                            fun_and_new_value_.second : std::forward<Other>(other);
                    }
                };
            public:
                template<typename Rng, typename Pred, typename Val>
                transformed_view<Rng, replacer_if_fun<Pred, detail::decay_t<Val>>>
                operator()(Rng && rng, Pred pred, Val new_value) const
                {
                    CONCEPT_ASSERT(InputIterable<Rng>());
                    CONCEPT_ASSERT(InvokablePredicate<Pred, range_value_t<Rng>>());
                    CONCEPT_ASSERT(Convertible<Val, range_value_t<Rng>>());
                    return {std::forward<Rng>(rng), {std::move(pred), std::move(new_value)}};

                }

                template<typename Pred, typename Val>
                auto operator()(Pred pred, Val new_value) const ->
                    decltype(make_pipeable(std::bind(*this, std::placeholders::_1, protect(std::move(pred)),
                        std::move(new_value))))
                {
                    return make_pipeable(std::bind(*this, std::placeholders::_1, protect(std::move(pred)),
                        std::move(new_value)));
                }
            };

            constexpr replace_if_fn replace_if {};
        }
    }
}

#endif
