// Range v3 library
//
//  Copyright Eric Niebler 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
#ifndef RANGES_V3_VIEW_BOUNDED_HPP
#define RANGES_V3_VIEW_BOUNDED_HPP

#include <type_traits>
#include <range/v3/range_fwd.hpp>
#include <range/v3/size.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_facade.hpp>
#include <range/v3/range_interface.hpp>
#include <range/v3/utility/bindable.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/common_range_iterator.hpp>
#include <range/v3/view/all.hpp>

namespace ranges
{
    inline namespace v3
    {
        template<typename Rng>
        struct bounded_view
          : range_interface<bounded_view<Rng>>
        {
        private:
            friend range_access;
            using base_range_t = view::all_t<Rng>;
            using base_iterator_t = range_iterator_t<base_range_t>;
            using base_sentinel_t = range_sentinel_t<base_range_t>;
            base_range_t rng_;

        public:
            using iterator = common_range_iterator<base_iterator_t, base_sentinel_t>;

            bounded_view() = default;
            explicit bounded_view(Rng && rng)
              : rng_(view::all(std::forward<Rng>(rng)))
            {}
            iterator begin()
            {
                return iterator{ranges::begin(rng_)};
            }
            iterator end()
            {
                return iterator{ranges::end(rng_)};
            }
            CONCEPT_REQUIRES(Range<base_range_t const>())
            iterator begin() const
            {
                return iterator{ranges::begin(rng_)};
            }
            CONCEPT_REQUIRES(Range<base_range_t const>())
            iterator end() const
            {
                return iterator{ranges::end(rng_)};
            }
            CONCEPT_REQUIRES(SizedRange<base_range_t>())
            range_size_t<base_range_t> size() const
            {
                return ranges::size(rng_);
            }
        };

        namespace view
        {
            struct bounded_fn : bindable<bounded_fn>, pipeable<bounded_fn>
            {
                template<typename Rng>
                static bounded_view<Rng>
                invoke(bounded_fn, Rng && rng)
                {
                    CONCEPT_ASSERT(InputIterable<Rng>());
                    CONCEPT_ASSERT(!BoundedIterable<Rng>());
                    return bounded_view<Rng>{std::forward<Rng>(rng)};
                }
            };

            RANGES_CONSTEXPR bounded_fn bounded{};
        }
    }
}

#endif
