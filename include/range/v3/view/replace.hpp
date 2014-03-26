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

#ifndef RANGES_V3_VIEW_REPLACE_HPP
#define RANGES_V3_VIEW_REPLACE_HPP

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
            struct replacer : bindable<replacer>
            {
            private:
                template<typename Value>
                struct replacer_fun
                {
                private:
                    friend struct replacer;
                    Value old_value_;
                    Value new_value_;

                    template<typename Value1, typename Value2>
                    replacer_fun(Value1 && old_value, Value2 && new_value)
                      : old_value_(std::forward<Value1>(old_value)),
                        new_value_(std::forward<Value2>(new_value))
                    {}

                public:
                    template<typename Other>
                    Other operator()(Other && other) const
                    {
                        return (other == old_value_) ? new_value_ : std::forward<Other>(other);
                    }
                };
            public:
                template<typename InputIterable, typename Value1, typename Value2,
                    CONCEPT_REQUIRES_(ranges::Same<typename std::decay<Value1>::type,
                                                      typename std::decay<Value2>::type>())>
                static transformed_view<InputIterable,
                                            replacer_fun<typename std::decay<Value1>::type>>
                invoke(replacer, InputIterable && rng, Value1 && old_value, Value2 && new_value)
                {
                    CONCEPT_ASSERT(ranges::Iterable<InputIterable>());
                    CONCEPT_ASSERT(ranges::InputIterator<range_iterator_t<InputIterable>>());
                    CONCEPT_ASSERT(ranges::EqualityComparable<range_reference_t<InputIterable>,
                        typename std::decay<Value1>::type const &>());
                    CONCEPT_ASSERT(ranges::Convertible<typename std::decay<Value1>::type const &,
                        range_reference_t<InputIterable>>());
                    return {std::forward<InputIterable>(rng),
                            {std::forward<Value1>(old_value),
                             std::forward<Value2>(new_value)}};

                }

                /// \overload
                template<typename Value1, typename Value2,
                    CONCEPT_REQUIRES_(ranges::Same<typename std::decay<Value1>::type,
                                                      typename std::decay<Value2>::type>())>
                static auto invoke(replacer replace, Value1 && old_value, Value2 && new_value) ->
                    decltype(replace.move_bind(std::placeholders::_1, std::forward<Value1>(old_value),
                        std::forward<Value2>(new_value)))
                {
                    return replace.move_bind(std::placeholders::_1, std::forward<Value1>(old_value),
                        std::forward<Value2>(new_value));
                }
            };

            RANGES_CONSTEXPR replacer replace {};
        }
    }
}

#endif
