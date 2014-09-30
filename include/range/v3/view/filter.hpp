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

#ifndef RANGES_V3_VIEW_FILTER_HPP
#define RANGES_V3_VIEW_FILTER_HPP

#include <utility>
#include <type_traits>
#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/range_facade.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/utility/bindable.hpp>
#include <range/v3/utility/invokable.hpp>

namespace ranges
{
    inline namespace v3
    {
        template<typename Rng, typename Pred>
        struct filtered_view
          : range_adaptor<filtered_view<Rng, Pred>, Rng>
        {
        private:
            friend range_access;
            using base_cursor_t = ranges::base_cursor_t<filtered_view>;
            invokable_t<Pred> pred_;

            struct adaptor : default_adaptor
            {
            private:
                filtered_view const *rng_;
                using default_adaptor::advance;
                void satisfy(base_cursor_t &pos) const
                {
                    auto const end = default_adaptor::end(*rng_);
                    while(!end.equal(pos) && !rng_->pred_(pos.current()))
                        pos.next();
                }
            public:
                adaptor() = default;
                adaptor(filtered_view const &rng)
                  : rng_(&rng)
                {}
                base_cursor_t begin(filtered_view const &rng) const
                {
                    auto pos = default_adaptor::begin(rng);
                    this->satisfy(pos);
                    return pos;
                }
                void next(base_cursor_t &pos) const
                {
                    pos.next();
                    this->satisfy(pos);
                }
                CONCEPT_REQUIRES(BidirectionalIterable<Rng>())
                void prev(base_cursor_t &pos) const
                {
                    do
                    {
                        pos.prev();
                    } while (!rng_->pred_(pos.current()));
                }
            };
            adaptor begin_adaptor() const
            {
                return {*this};
            }
            // TODO: if end is a sentinel, it hold an unnecessary pointer back to
            // this range.
            adaptor end_adaptor() const
            {
                return {*this};
            }
        public:
            filtered_view(Rng && rng, Pred pred)
              : range_adaptor_t<filtered_view>(std::forward<Rng>(rng))
              , pred_(invokable(std::move(pred)))
            {}
        };

        namespace view
        {
            struct filter_fn : bindable<filter_fn>
            {
            private:
                template<typename Pred>
                struct filterer1 : pipeable<filterer1<Pred>>
                {
                private:
                    Pred pred_;
                public:
                    filterer1(Pred pred)
                      : pred_(std::move(pred))
                    {}
                    template<typename Rng, typename This>
                    static filtered_view<Rng, Pred> pipe(Rng && rng, This && this_)
                    {
                        CONCEPT_ASSERT(Iterable<Rng>());
                        CONCEPT_ASSERT(InvokablePredicate<Pred, range_value_t<Rng>>());
                        return {std::forward<Rng>(rng), std::forward<This>(this_).pred_};
                    }
                };
            public:
                template<typename Rng, typename Pred>
                static filtered_view<Rng, Pred>
                invoke(filter_fn, Rng && rng, Pred pred)
                {
                    CONCEPT_ASSERT(Iterable<Rng>());
                    CONCEPT_ASSERT(InvokablePredicate<Pred, range_value_t<Rng>>());
                    return {std::forward<Rng>(rng), std::move(pred)};
                }
                template<typename Pred>
                static filterer1<Pred> invoke(filter_fn, Pred pred)
                {
                    return {std::move(pred)};
                }
            };

            RANGES_CONSTEXPR filter_fn filter {};
        }
    }
}

#endif
