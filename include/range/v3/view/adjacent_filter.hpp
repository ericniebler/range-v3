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
#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_adaptor.hpp>
#include <range/v3/utility/bindable.hpp>
#include <range/v3/utility/iterator.hpp>

namespace ranges
{
    inline namespace v3
    {
        template<typename Rng, typename F>
        struct adjacent_filtered_view
          : range_adaptor<adjacent_filtered_view<Rng, F>, Rng>
        {
        private:
            friend range_core_access;
            invokable_t<F> pred_;
            using base_cursor_t = ranges::base_cursor_t<adjacent_filtered_view>;

            struct adaptor : default_adaptor
            {
            private:
                adjacent_filtered_view const *rng_;
                using default_adaptor::prev;
            public:
                adaptor() = default;
                adaptor(adjacent_filtered_view const &rng)
                  : rng_(&rng)
                {}
                void next(base_cursor_t &pos) const
                {
                    auto const end = default_adaptor::end(*rng_);
                    RANGES_ASSERT(!end.equal(pos));
                    auto const &pred = rng_->pred_;
                    auto const prev = pos;
                    do pos.next(); while (!end.equal(pos) && !pred(prev.current(), pos.current()));
                }
            };
            adaptor begin_adaptor() const
            {
                return{*this};
            }
            adaptor end_adaptor() const
            {
                return{*this};
            }
        public:
            adjacent_filtered_view(Rng && rng, F pred)
              : range_adaptor_t<adjacent_filtered_view>{std::forward<Rng>(rng)}
              , pred_(invokable(std::move(pred)))
            {}
        };

        namespace view
        {
            struct adjacent_filter_fn : bindable<adjacent_filter_fn>
            {
                template<typename Rng, typename F>
                static adjacent_filtered_view<Rng, F>
                invoke(adjacent_filter_fn, Rng && rng, F pred)
                {
                    CONCEPT_ASSERT(ForwardIterable<Rng>());
                    CONCEPT_ASSERT(InvokablePredicate<F, range_value_t<Rng>,
                                                         range_value_t<Rng>>());
                    return {std::forward<Rng>(rng), std::move(pred)};
                }
                template<typename F>
                static auto invoke(adjacent_filter_fn adjacent_filter, F pred) ->
                    decltype(adjacent_filter.move_bind(std::placeholders::_1, std::move(pred)))
                {
                    return adjacent_filter.move_bind(std::placeholders::_1, std::move(pred));
                }
            };

            RANGES_CONSTEXPR adjacent_filter_fn adjacent_filter {};
        }
    }
}

#endif
