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
#include <range/v3/iterator_range.hpp>
#include <range/v3/utility/unreachable.hpp>
#include <range/v3/utility/iterator_concepts.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace detail
        {
            template<typename Value>
            struct delimit_sentinel_adaptor : default_adaptor
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
        struct delimited_view
          : range_adaptor<delimited_view<InputIterable, Value>, InputIterable>
        {
        private:
            friend range_core_access;
            Value value_;

            default_adaptor begin_adaptor() const
            {
                return {};
            }
            detail::delimit_sentinel_adaptor<Value> end_adaptor() const
            {
                return {value_};
            }
        public:
            delimited_view(InputIterable && rng, Value value)
              : range_adaptor_t<delimited_view>(std::forward<InputIterable>(rng))
              , value_(std::move(value))
            {}
        };

        namespace view
        {
            struct delimiter : bindable<delimiter>
            {
                template<typename InputIterable, typename Value,
                    CONCEPT_REQUIRES_(ranges::Iterable<InputIterable>())>
                static delimited_view<InputIterable, Value>
                invoke(delimiter, InputIterable && rng, Value value)
                {
                    return {std::forward<InputIterable>(rng), std::move(value)};
                }

                template<typename InputIterator, typename Value,
                    CONCEPT_REQUIRES_(ranges::InputIterator<InputIterator>())>
                static delimited_view<iterator_range<InputIterator, unreachable>, Value>
                invoke(delimiter, InputIterator begin, Value value)
                {
                    return {{std::move(begin), {}}, std::move(value)};
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
