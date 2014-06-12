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

#ifndef RANGES_V3_VIEW_TAIL_HPP
#define RANGES_V3_VIEW_TAIL_HPP

#include <utility>
#include <type_traits>
#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/empty.hpp>
#include <range/v3/size.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_facade.hpp>
#include <range/v3/utility/iterator.hpp>
#include <range/v3/utility/bindable.hpp>

namespace ranges
{
    inline namespace v3
    {
        template<typename Rng>
        struct tail_view
          : private range_base
        {
        private:
            detail::base_iterable_holder<Rng> rng_;
        public:
            using iterator = range_iterator_t<Rng>;
            using sentinel = range_sentinel_t<Rng>;

            tail_view() = default;
            tail_view(Rng &&rng)
              : rng_(std::forward<Rng>(rng))
            {
                CONCEPT_ASSERT(InputIterable<Rng>());
                RANGES_ASSERT(!ForwardIterable<Rng>() || !empty(rng_.get()));
            }
            iterator begin() const
            {
                return next(ranges::begin(rng_.get()));
            }
            sentinel end() const
            {
                return ranges::end(rng_.get());
            }
            bool operator!() const
            {
                return begin() == end();
            }
            explicit operator bool() const
            {
                return begin() != end();
            }
            Rng & base()
            {
                return rng_.get();
            }
            Rng const & base() const
            {
                return rng_.get();
            }
            CONCEPT_REQUIRES(SizedIterable<Rng>())
            range_size_t<Rng> size() const
            {
                return ranges::size(rng_.get()) - 1;
            }
        };

        namespace view
        {
            struct tail_fn : bindable<tail_fn>, pipeable<tail_fn>
            {
                template<typename Rng>
                static tail_view<Rng> invoke(tail_fn, Rng && rng)
                {
                    CONCEPT_ASSERT(Iterable<Rng>());
                    return tail_view<Rng>{std::forward<Rng>(rng)};
                }
            };

            RANGES_CONSTEXPR tail_fn tail {};
        }
    }
}

#endif
