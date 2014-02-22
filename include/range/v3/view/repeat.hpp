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
        template<typename Value>
        struct repeat_iterable_view
          : range_facade<repeat_iterable_view<Value>>
        {
        private:
            Value value_;
            friend struct range_facade<repeat_iterable_view>;
            using is_infinite = std::true_type;

            struct impl
            {
                repeat_iterable_view const *view_;
                impl(repeat_iterable_view const *view)
                  : view_(view)
                {}
                Value const &current() const
                {
                    return view_->value_;
                }
                constexpr bool done() const
                {
                    return false;
                }
                void next() const
                {}
            };
            impl begin_impl() const
            {
                return {this};
            }
        public:
            repeat_iterable_view() = default;
            constexpr explicit repeat_iterable_view(Value value)
              : value_(detail::move(value))
            {}
        };

        namespace view
        {
            struct repeater : bindable<repeater>, pipeable<repeater>
            {
                template<typename Value>
                static repeat_iterable_view<Value> invoke(repeater, Value value)
                {
                    return repeat_iterable_view<Value>{std::move(value)};
                }
            };

            RANGES_CONSTEXPR repeater repeat{};
        }
    }
}

#endif
