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
            template<typename Val>
            struct delimit_sentinel_adaptor : default_adaptor
            {
            private:
                Val value_;
            public:
                delimit_sentinel_adaptor() = default;
                delimit_sentinel_adaptor(Val value)
                  : value_(std::move(value))
                {}
                template<typename Cur, typename S>
                bool empty(Cur const &pos, S const &end) const
                {
                    return end.equal(pos) || pos.current() == value_;
                }
            };
        }

        template<typename Rng, typename Val>
        struct delimited_view
          : range_adaptor<delimited_view<Rng, Val>, Rng>
        {
        private:
            friend range_core_access;
            Val value_;

            default_adaptor begin_adaptor() const
            {
                return {};
            }
            detail::delimit_sentinel_adaptor<Val> end_adaptor() const
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
                    CONCEPT_REQUIRES_(ConvertibleToRange<Rng>())>
                static delimited_view<Rng, Val>
                invoke(delimit_fn, Rng && rng, Val value)
                {
                    return {std::forward<Rng>(rng), std::move(value)};
                }

                template<typename I, typename Val,
                    CONCEPT_REQUIRES_(InputIterator<I>())>
                static delimited_view<iterator_range<I, unreachable>, Val>
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
