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
        template<typename Rng>
        struct reversed_view
          : range_adaptor<reversed_view<Rng>, Rng>
        {
        private:
            CONCEPT_ASSERT(BidirectionalRange<Rng>());
            friend range_core_access;
            using base_cursor_t = ranges::base_cursor_t<reversed_view>;

            // A rather convoluted implementation to avoid the problem std::reverse_iterator
            // has adapting iterators that return references to internal data.
            struct adaptor : default_adaptor
            {
            private:
                reversed_view const *rng_;
            public:
                adaptor() = default;
                adaptor(reversed_view const &rng)
                  : rng_(&rng)
                {}
                base_cursor_t begin(reversed_view const &rng) const
                {
                    auto pos = default_adaptor::end(rng);
                    if(!pos.equal(default_adaptor::begin(rng)))
                        pos.prev();
                    return pos;
                }
                void next(base_cursor_t &pos) const
                {
                    if(pos.equal(default_adaptor::begin(*rng_)))
                        pos = default_adaptor::end(*rng_);
                    else
                        pos.prev();
                }
                void prev(base_cursor_t &pos) const
                {
                    if(pos.equal(default_adaptor::end(*rng_)))
                        pos = default_adaptor::begin(*rng_);
                    else
                        pos.next();
                }
                CONCEPT_REQUIRES(RandomAccessRange<Rng>())
                void advance(base_cursor_t &pos, range_difference_t<Rng> n) const
                {
                    if(n > 0)
                        pos.advance(-n + 1), next(pos);
                    else if(n < 0)
                        prev(pos), pos.advance(-n - 1);
                }
                CONCEPT_REQUIRES(RandomAccessRange<Rng>())
                range_difference_t<Rng>
                distance_to(base_cursor_t const &here, base_cursor_t const &there) const
                {
                    if(there.equal(default_adaptor::end(*rng_)))
                        return here.equal(default_adaptor::end(*rng_))
                            ? 0 : default_adaptor::begin(*rng_).distance_to(here) + 1;
                    else if(here.equal(default_adaptor::end(*rng_)))
                        return there.distance_to(default_adaptor::begin(*rng_)) - 1;
                    return there.distance_to(here);
                }
            };
            adaptor begin_adaptor() const
            {
                return {*this};
            }
            adaptor end_adaptor() const
            {
                return {*this};
            }
        public:
            reversed_view() = default;
            reversed_view(Rng && rng)
              : range_adaptor_t<reversed_view>(std::forward<Rng>(rng))
            {}
            CONCEPT_REQUIRES(SizedIterable<Rng>())
            range_size_t<Rng> size() const
            {
                return this->base_size();
            }
        };

        namespace view
        {
            struct reverse_fn : bindable<reverse_fn>, pipeable<reverse_fn>
            {
                template<typename Rng>
                static reversed_view<Rng>
                invoke(reverse_fn, Rng && rng)
                {
                    CONCEPT_ASSERT(BidirectionalRange<Rng>());
                    return reversed_view<Rng>{std::forward<Rng>(rng)};
                }
            };

            RANGES_CONSTEXPR reverse_fn reverse {};
        }
    }
}

#endif
