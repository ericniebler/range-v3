// Range v3 library
//
//  Copyright Eric Niebler 2013-2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGES_V3_VIEW_DELIMIT_HPP
#define RANGES_V3_VIEW_DELIMIT_HPP

#include <range/v3/range_fwd.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_adaptor.hpp>
#include <range/v3/range.hpp>
#include <range/v3/utility/unreachable.hpp>
#include <range/v3/utility/iterator_concepts.hpp>

namespace ranges
{
    inline namespace v3
    {
        template<typename Rng, typename Val>
        struct delimited_view
          : range_adaptor<delimited_view<Rng, Val>, Rng>
        {
        private:
            friend range_access;
            Val value_;

            struct sentinel_adaptor : sentinel_adaptor_base
            {
                sentinel_adaptor() = default;
                sentinel_adaptor(Val value)
                  : value_(std::move(value))
                {}
                bool empty(range_iterator_t<Rng> it, range_sentinel_t<Rng> end) const
                {
                    return it == end || *it == value_;
                }
                Val value_;
            };

            sentinel_adaptor end_adaptor() const
            {
                return {value_};
            }
        public:
            delimited_view() = default;
            delimited_view(Rng && rng, Val value)
              : range_adaptor_t<delimited_view>(std::forward<Rng>(rng))
              , value_(std::move(value))
            {}
        };

        namespace view
        {
            struct delimit_fn : bindable<delimit_fn>
            {
                template<typename Rng, typename Val ,
                    CONCEPT_REQUIRES_(Iterable<Rng>())>
                static delimited_view<Rng, Val>
                invoke(delimit_fn, Rng && rng, Val value)
                {
                    return {std::forward<Rng>(rng), std::move(value)};
                }

                template<typename I, typename Val,
                    CONCEPT_REQUIRES_(InputIterator<I>())>
                static delimited_view<range<I, unreachable>, Val>
                invoke(delimit_fn, I begin, Val value)
                {
                    return {{std::move(begin), {}}, std::move(value)};
                }

                template<typename Val>
                static auto
                invoke(delimit_fn delimit, Val value) ->
                    decltype(delimit.move_bind(std::placeholders::_1, std::move(value)))
                {
                    return delimit.move_bind(std::placeholders::_1, std::move(value));
                }
            };

            RANGES_CONSTEXPR delimit_fn delimit{};
        }
    }
}

#endif
