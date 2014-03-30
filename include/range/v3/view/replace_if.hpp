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
            struct replacer_if : bindable<replacer_if>
            {
            private:
                template<typename UnaryPredicate, typename Value>
                struct replacer_if_fun
                {
                private:
                    friend struct replacer_if;
                    compressed_pair<invokable_t<UnaryPredicate>, Value> fun_and_new_value_;

                    template<typename Value2>
                    replacer_if_fun(UnaryPredicate pred, Value2 && new_value)
                      : fun_and_new_value_{make_invokable(std::move(pred)),
                                           std::forward<Value2>(new_value)}
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
                template<typename InputIterable, typename UnaryPredicate, typename Value>
                static transformed_view<InputIterable,
                                               replacer_if_fun<UnaryPredicate,
                                                    typename std::decay<Value>::type>>
                invoke(replacer_if, InputIterable && rng, UnaryPredicate pred, Value && new_value)
                {
                    CONCEPT_ASSERT(ranges::Iterable<InputIterable>());
                    CONCEPT_ASSERT(ranges::InputIterator<range_iterator_t<InputIterable>>());
                    CONCEPT_ASSERT(ranges::InvokablePredicate<UnaryPredicate,
                                                              range_value_t<InputIterable>>());
                    CONCEPT_ASSERT(ranges::Convertible<detail::decay_t<Value> const &,
                                                       range_reference_t<InputIterable>>());
                    return {std::forward<InputIterable>(rng),
                            {std::move(pred), std::forward<Value>(new_value)}};

                }

                /// \overload
                template<typename UnaryPredicate, typename Value>
                static auto
                invoke(replacer_if replace_if, UnaryPredicate pred, Value && new_value) ->
                    decltype(replace_if.move_bind(std::placeholders::_1, std::move(pred),
                        std::forward<Value>(new_value)))
                {
                    return replace_if.move_bind(std::placeholders::_1, std::move(pred),
                        std::forward<Value>(new_value));
                }
            };

            RANGES_CONSTEXPR replacer_if replace_if {};
        }
    }
}

#endif
