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
#include <range/v3/utility/iterator_facade.hpp>
#include <range/v3/utility/sentinel_facade.hpp>
#include <range/v3/utility/infinity.hpp>

namespace ranges
{
    inline namespace v3
    {
        template<typename Value>
        struct repeat_iterable_view
        {
        private:
            Value value_;

        public:
            struct sentinel;
            using const_iterator = struct iterator
              : ranges::iterator_facade<
                    iterator
                  , Value
                  , std::input_iterator_tag
                  , Value const &
                >
            {
            private:
                friend struct repeat_iterable_view;
                friend struct iterator_core_access;
                Value value_;

                explicit iterator(Value value)
                  : value_(std::move(value))
                {}
                Value const &dereference() const
                {
                    return value_;
                }
                constexpr bool equal(iterator const &) const
                {
                    return true;
                }
                constexpr bool equal(sentinel) const
                {
                    return false;
                }
                void increment()
                {}
                constexpr infinity distance_to(sentinel) const
                {
                    return {};
                }
            public:
                constexpr iterator()
                  : value_{}
                {}
            };
            using const_sentinel = struct sentinel
              : ranges::sentinel_facade<sentinel, iterator>
            {};

            explicit repeat_iterable_view(Value value)
              : value_(std::move(value))
            {}

            iterator begin() const
            {
                return iterator{value_};
            }
            sentinel end() const
            {
                return {};
            }
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
