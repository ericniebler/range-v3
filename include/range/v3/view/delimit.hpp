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
          : range_adaptor<delimit_iterable_view<InputIterable, Value>, InputIterable>
        {
        private:
            friend struct range_core_access;
            using base_t = range_adaptor<delimit_iterable_view, InputIterable>;
            template<bool Const>
            using basic_impl_t = range_core_access::basic_impl_t<base_t, Const>;
            template<bool Const>
            using sentinel_base_t = range_core_access::basic_sentinel_t<base_t, Const>;
            Value value_;

            template<bool Const>
            struct basic_sentinel : sentinel_base_t<Const>
            {
                Value const *value_;
                basic_sentinel() = default;
                basic_sentinel(sentinel_base_t<Const> base, Value const *value)
                  : sentinel_base_t<Const>(std::move(base)), value_(value)
                {}
                // For sentinel -> const_sentinel conversion
                template<bool OtherConst, typename std::enable_if<!OtherConst, int>::type = 0>
                basic_sentinel(basic_sentinel<OtherConst> that)
                  : sentinel_base_t<Const>(std::move(that)), value_(that.value_)
                {}
                template<bool OtherConst>
                bool done(basic_impl_t<OtherConst> const &that) const
                {
                    return this->base().equal(that) ||
                           that.current() == *value_;
                }
            };

            basic_sentinel<false> end_impl()
            {
                return {this->adaptor().end_impl(), &value_};
            }
            basic_sentinel<true> end_impl() const
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
