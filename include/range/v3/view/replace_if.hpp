// Boost.Range library
//
//  Copyright Thorsten Ottosen, Neil Groves 2006 - 2008.
//  Copyright Eric Niebler 2013.
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//

#ifndef RANGES_V3_VIEW_REPLACE_IF_HPP
#define RANGES_V3_VIEW_REPLACE_IF_HPP

#include <utility>
#include <type_traits>
#include <range/v3/range_fwd.hpp>
#include <range/v3/view/transform.hpp>
#include <range/v3/utility/bindable.hpp>
#include <range/v3/utility/concepts.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace view
        {
            struct replace_if_fn : bindable<replace_if_fn>
            {
            private:
                template<typename Pred, typename Val>
                struct replacer_if_fun
                {
                private:
                    friend struct replace_if_fn;
                    compressed_pair<invokable_t<Pred>, Val> fun_and_new_value_;

                    template<typename Val2>
                    replacer_if_fun(Pred pred, Val2 && new_value)
                      : fun_and_new_value_{invokable(std::move(pred)),
                                           std::forward<Val2>(new_value)}
                    {}
                public:
                    template<typename Other>
                    Other operator()(Other && other) const
                    {
                        return (fun_and_new_value_.first()(other)) ?
                            fun_and_new_value_.second() : std::forward<Other>(other);
                    }
                };
            public:
                template<typename Rng, typename Pred, typename Val>
                static transformed_view<Rng,
                                        replacer_if_fun<Pred,
                                                        detail::decay_t<Val>>>
                invoke(replace_if_fn, Rng && rng, Pred pred, Val && new_value)
                {
                    CONCEPT_ASSERT(InputRange<Rng>());
                    CONCEPT_ASSERT(InvokablePredicate<Pred,
                                                      range_value_t<Rng>>());
                    CONCEPT_ASSERT(Convertible<detail::decay_t<Val> const &,
                                               range_reference_t<Rng>>());
                    return {std::forward<Rng>(rng),
                            {std::move(pred), std::forward<Val>(new_value)}};

                }

                /// \overload
                template<typename Pred, typename Val>
                static auto
                invoke(replace_if_fn replace_if, Pred pred, Val && new_value) ->
                    decltype(replace_if.move_bind(std::placeholders::_1, std::move(pred),
                        std::forward<Val>(new_value)))
                {
                    return replace_if.move_bind(std::placeholders::_1, std::move(pred),
                        std::forward<Val>(new_value));
                }
            };

            RANGES_CONSTEXPR replace_if_fn replace_if {};
        }
    }
}

#endif
