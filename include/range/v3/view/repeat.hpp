// Boost.Range library
//
//  Copyright Eric Niebler 2013.
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//

#ifndef RANGES_V3_VIEW_REPEAT_HPP
#define RANGES_V3_VIEW_REPEAT_HPP

#include <range/v3/range_fwd.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_facade.hpp>

namespace ranges
{
    inline namespace v3
    {
        // BUGBUG a view shouldn't contain its value, right?
        template<typename Value>
        struct repeated_view
          : range_facade<repeated_view<Value>, true>
        {
        private:
            Value value_;
            friend range_core_access;

            struct cursor
            {
                Value const *value_;
                Value const &current() const
                {
                    return *value_;
                }
                constexpr bool done() const
                {
                    return false;
                }
                void next() const
                {}
            };
            cursor begin_cursor() const
            {
                return {&value_};
            }
        public:
            repeated_view() = default;
            constexpr explicit repeated_view(Value value)
              : value_(detail::move(value))
            {}
        };

        namespace view
        {
            struct repeater : bindable<repeater>, pipeable<repeater>
            {
                template<typename Value>
                static repeated_view<Value> invoke(repeater, Value value)
                {
                    return repeated_view<Value>{std::move(value)};
                }
            };

            RANGES_CONSTEXPR repeater repeat{};
        }
    }
}

#endif
