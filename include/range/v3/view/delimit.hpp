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
        template<typename InputIterable, typename Value>
        struct delimit_iterable_view
          : range_adaptor<
                delimit_iterable_view<InputIterable, Value>,
                InputIterable>
        {
        private:
            friend range_core_access;
            using base_t = range_adaptor_t<delimit_iterable_view>;
            using sentinel_base_t = basic_adaptor_sentinel<InputIterable>;
            Value value_;

            struct sentinel : sentinel_base_t
            {
                Value const *value_;
                using sentinel_base_t::sentinel_base_t;
                sentinel(sentinel_base_t base, Value const *value)
                  : sentinel_base_t(std::move(base)), value_(value)
                {}
                bool equal(basic_adaptor_impl<InputIterable> const &that) const
                {
                    return this->base().equal(that) ||
                           that.current() == *value_;
                }
            };
            sentinel end_impl() const
            {
                return {this->adaptor().end_impl(), &value_};
            }
        public:
            delimit_iterable_view(InputIterable && rng, Value value)
              : base_t(std::forward<InputIterable>(rng)), value_(std::move(value))
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
