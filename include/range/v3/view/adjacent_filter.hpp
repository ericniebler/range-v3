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

#ifndef RANGES_V3_VIEW_ADJACENT_FILTER_HPP
#define RANGES_V3_VIEW_ADJACENT_FILTER_HPP

#include <utility>
#include <range/v3/next_prev.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/utility/bindable.hpp>
#include <range/v3/range_adaptor.hpp>

namespace ranges
{
    inline namespace v3
    {
        template<typename ForwardIterable, typename BinaryPredicate>
        struct adjacent_filter_range_view
          : range_adaptor<adjacent_filter_range_view<ForwardIterable, BinaryPredicate>,
                          ForwardIterable>
        {
        private:
            friend range_core_access;
            invokable_t<BinaryPredicate> pred_;
            using base_cursor_t = base_cursor_t<adjacent_filter_range_view>;

            struct adaptor : adaptor_defaults
            {
            private:
                adjacent_filter_range_view const *rng_;
                using adaptor_defaults::prev;
            public:
                adaptor() = default;
                adaptor(adjacent_filter_range_view const &rng)
                  : rng_(&rng)
                {}
                void next(base_cursor_t &pos) const
                {
                    auto const end = adaptor_defaults::end(*rng_);
                    RANGES_ASSERT(!end.equal(pos));
                    auto const &pred = rng_->pred_;
                    auto const prev = pos;
                    do pos.next(); while (!end.equal(pos) && !pred(prev.current(), pos.current()));
                }
            };
            adaptor get_adaptor(begin_end_tag) const
            {
                return {*this};
            }
        public:
            adjacent_filter_range_view(ForwardIterable && rng, BinaryPredicate pred)
              : range_adaptor_t<adjacent_filter_range_view>{std::forward<ForwardIterable>(rng)}
              , pred_(make_invokable(std::move(pred)))
            {}
        };

        namespace view
        {
            struct adjacent_filterer : bindable<adjacent_filterer>
            {
                template<typename ForwardIterable, typename BinaryPredicate>
                static adjacent_filter_range_view<ForwardIterable, BinaryPredicate>
                invoke(adjacent_filterer, ForwardIterable && rng, BinaryPredicate pred)
                {
                    CONCEPT_ASSERT(ranges::Range<ForwardIterable>());
                    CONCEPT_ASSERT(ranges::ForwardIterator<range_iterator_t<ForwardIterable>>());
                    CONCEPT_ASSERT(ranges::BinaryPredicate<invokable_t<BinaryPredicate>,
                                                           range_reference_t<ForwardIterable>,
                                                           range_reference_t<ForwardIterable>>());
                    return {std::forward<ForwardIterable>(rng), std::move(pred)};
                }
                template<typename BinaryPredicate>
                static auto invoke(adjacent_filterer adjacent_filter, BinaryPredicate pred) ->
                    decltype(adjacent_filter.move_bind(std::placeholders::_1, std::move(pred)))
                {
                    return adjacent_filter.move_bind(std::placeholders::_1, std::move(pred));
                }
            };

            RANGES_CONSTEXPR adjacent_filterer adjacent_filter {};
        }
    }
}

#endif
