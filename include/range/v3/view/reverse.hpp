// Boost.Range library
//
//  Copyright Eric Niebler 2014.
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//

#ifndef RANGES_V3_VIEW_REVERSE_HPP
#define RANGES_V3_VIEW_REVERSE_HPP

#include <utility>
#include <iterator>
#include <range/v3/range_fwd.hpp>
#include <range/v3/size.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/range_adaptor.hpp>
#include <range/v3/utility/bindable.hpp>

namespace ranges
{
    inline namespace v3
    {
        template<typename BidirectionalRange>
        struct reverse_range_view
          : range_adaptor<reverse_range_view<BidirectionalRange>, BidirectionalRange>
        {
        private:
            CONCEPT_ASSERT(ranges::Range<BidirectionalRange>());
            CONCEPT_ASSERT(ranges::BidirectionalIterator<ranges::range_iterator_t<BidirectionalRange>>());
            friend range_core_access;
            using base_cursor_t = ranges::base_cursor_t<reverse_range_view>;

            // A rather convoluted implementation to avoid the problem std::reverse_iterator
            // has adapting iterators that return references to internal data.
            struct adaptor : adaptor_defaults
            {
            private:
                reverse_range_view const *rng_;
            public:
                adaptor() = default;
                adaptor(reverse_range_view const &rng)
                  : rng_(&rng)
                {}
                base_cursor_t begin(reverse_range_view const &rng) const
                {
                    auto pos = adaptor_defaults::end(rng);
                    if(!pos.equal(adaptor_defaults::begin(rng)))
                        pos.prev();
                    return pos;
                }
                void next(base_cursor_t &pos) const
                {
                    if(pos.equal(adaptor_defaults::begin(*rng_)))
                        pos = adaptor_defaults::end(*rng_);
                    else
                        pos.prev();
                }
                void prev(base_cursor_t &pos) const
                {
                    if(pos.equal(adaptor_defaults::end(*rng_)))
                        pos = adaptor_defaults::begin(*rng_);
                    else
                        pos.next();
                }
                CONCEPT_REQUIRES(ranges::RandomAccessIterator<ranges::range_iterator_t<BidirectionalRange>>())
                void advance(base_cursor_t &pos, ranges::range_difference_t<BidirectionalRange> n) const
                {
                    if(n > 0)
                        pos.advance(-n + 1), next(pos);
                    else if(n < 0)
                        prev(pos), pos.advance(-n - 1);
                }
                CONCEPT_REQUIRES(ranges::RandomAccessIterator<ranges::range_iterator_t<BidirectionalRange>>())
                ranges::range_difference_t<BidirectionalRange>
                distance_to(base_cursor_t const &here, base_cursor_t const &there) const
                {
                    if(there.equal(adaptor_defaults::end(*rng_)))
                        return here.equal(adaptor_defaults::end(*rng_))
                            ? 0 : adaptor_defaults::begin(*rng_).distance_to(here) + 1;
                    else if(here.equal(adaptor_defaults::end(*rng_)))
                        return there.distance_to(adaptor_defaults::begin(*rng_)) - 1;
                    return there.distance_to(here);
                }
            };
            adaptor get_adaptor(ranges::begin_end_tag) const
            {
                return {*this};
            }
        public:
            reverse_range_view() = default;
            reverse_range_view(BidirectionalRange && rng)
              : range_adaptor_t<reverse_range_view>(std::forward<BidirectionalRange>(rng))
            {}
            CONCEPT_REQUIRES(ranges::SizedRange<BidirectionalRange>())
            range_size_t<BidirectionalRange> size() const
            {
                return this->base_size();
            }
        };

        namespace view
        {
            struct reverser : bindable<reverser>, pipeable<reverser>
            {
                template<typename BidirectionalRange>
                static reverse_range_view<BidirectionalRange>
                invoke(reverser, BidirectionalRange && rng)
                {
                    CONCEPT_ASSERT(ranges::Range<BidirectionalRange>());
                    CONCEPT_ASSERT(ranges::BidirectionalIterator<range_iterator_t<BidirectionalRange>>());
                    return reverse_range_view<BidirectionalRange>{
                        std::forward<BidirectionalRange>(rng)};
                }
            };

            RANGES_CONSTEXPR reverser reverse {};
        }
    }
}

#endif
