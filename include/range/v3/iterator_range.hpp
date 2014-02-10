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

#ifndef RANGES_V3_ITERATOR_RANGE_HPP
#define RANGES_V3_ITERATOR_RANGE_HPP

#include <utility>
#include <iterator>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/utility/bindable.hpp>
#include <range/v3/utility/compressed_pair.hpp>

namespace ranges
{
    inline namespace v3
    {
        // Intentionally resisting the urge to fatten this interface to make
        // it look like a container, like iterator_range. It's a range,
        // not a container.
        template<typename Iterator, typename Sentinel /* = Iterator */>
        struct iterator_range
        {
        private:
            compressed_pair<Iterator, Sentinel> begin_end_;
        public:
            using iterator = Iterator;
            using const_iterator = Iterator;
            using sentinel = Sentinel;
            using const_sentinel = Sentinel;

            iterator_range() = default;
            constexpr iterator_range(Iterator begin, Sentinel end)
              : begin_end_(detail::move(begin), detail::move(end))
            {}
            constexpr iterator_range(std::pair<Iterator, Sentinel> rng)
              : begin_end_(detail::move(rng.first), detail::move(rng.second))
            {}
            iterator begin() const
            {
                return begin_end_.first();
            }
            sentinel end() const
            {
                return begin_end_.second();
            }
            bool operator!() const
            {
                return begin_end_.first() == begin_end_.second();
            }
            explicit operator bool() const
            {
                return begin_end_.first() != begin_end_.second();
            }
            iterator_range & advance_begin(iterator_difference_t<Iterator> n)
            {
                std::advance(begin_end_.first(), n);
                return *this;
            }
            template<typename This = iterator_range,
                     CONCEPT_REQUIRES(ranges::Range<This>())>
            iterator_range & advance_end(iterator_difference_t<Iterator> n)
            {
                std::advance(begin_end_.second(), n);
                return *this;
            }
        };

        struct ranger : bindable<ranger>
        {
            template<typename Iterator, typename Sentinel>
            static iterator_range<Iterator> invoke(ranger, Iterator begin, Sentinel end)
            {
                CONCEPT_ASSERT(ranges::EqualityComparable<Iterator, Sentinel>());
                return {std::move(begin), std::move(end)};
            }
        };

        RANGES_CONSTEXPR ranger range {};
    }
}

#endif
