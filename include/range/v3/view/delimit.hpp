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

#ifndef RANGES_V3_VIEW_DELIMIT_HPP
#define RANGES_V3_VIEW_DELIMIT_HPP

#include <range/v3/range_fwd.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_adaptor.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace detail
        {
            template<typename Value>
            struct delimit_sentinel_adaptor : adaptor_defaults
            {
            private:
                Value value_;
            public:
                delimit_sentinel_adaptor() = default;
                delimit_sentinel_adaptor(Value value)
                  : value_(std::move(value))
                {}
                template<typename Cursor, typename Sentinel>
                bool empty(Cursor const &pos, Sentinel const &end) const
                {
                    return end.equal(pos) || pos.current() == value_;
                }
            };
        }

        template<typename InputIterable, typename Value>
        struct delimit_iterable_view
          : range_adaptor<delimit_iterable_view<InputIterable, Value>, InputIterable>
        {
        private:
            friend range_core_access;
            Value value_;

            using range_adaptor_t<delimit_iterable_view>::get_adaptor;
            detail::delimit_sentinel_adaptor<Value> get_adaptor(end_tag) const
            {
                return {value_};
            }
        public:
            delimit_iterable_view(InputIterable && rng, Value value)
              : range_adaptor_t<delimit_iterable_view>(std::forward<InputIterable>(rng))
              , value_(std::move(value))
            {}
        };

        namespace view
        {
            struct delimiter : bindable<delimiter>
            {
                template<typename InputIterable, typename Value>
                static delimit_iterable_view<InputIterable, Value>
                invoke(delimiter, InputIterable && rng, Value value)
                {
                    return {std::forward<InputIterable>(rng), std::move(value)};
                }

                template<typename Value>
                static auto
                invoke(delimiter delimit, Value value) ->
                    decltype(delimit.move_bind(std::placeholders::_1, std::move(value)))
                {
                    return delimit.move_bind(std::placeholders::_1, std::move(value));
                }
            };

            RANGES_CONSTEXPR delimiter delimit{};
        }
    }
}

#endif
