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

#ifndef RANGES_V3_RANGE_HPP
#define RANGES_V3_RANGE_HPP

#include <utility>
#include <type_traits>
#include <range/v3/range_fwd.hpp>
#include <range/v3/utility/bindable.hpp>
#include <range/v3/utility/iterator.hpp>
#include <range/v3/utility/compressed_pair.hpp>
#include <range/v3/utility/iterator_concepts.hpp>

namespace ranges
{
    inline namespace v3
    {
        // Intentionally resisting the urge to fatten this interface to make
        // it look like a container, like boost::range. It's a range,
        // not a container.
        template<typename I, typename S /* = I */>
        struct range
          : compressed_pair<I, S>
        {
            using iterator = I;
            using sentinel = S;

            range() = default;
            constexpr range(I begin, S end)
              : compressed_pair<I, S>{detail::move(begin), detail::move(end)}
            {}
            constexpr range(std::pair<I, S> rng)
              : compressed_pair<I, S>{detail::move(rng.first), detail::move(rng.second)}
            {}
            iterator begin() const
            {
                return this->first;
            }
            sentinel end() const
            {
                return this->second;
            }
            CONCEPT_REQUIRES(SizedIteratorRange<I, S>())
            iterator_size_t<I> size() const
            {
                return iterator_range_size(this->first, this->second);
            }
            bool operator!() const
            {
                return this->first == this->second;
            }
            explicit operator bool() const
            {
                return !!*this;
            }
        };

        // Like range, but with a known size. As with range and std::pair,
        // first and second are public members (for compatibility with old code using
        // pair to store iterator ranges), so mutating first or second directly without
        // mutating the size member can invalidate the class invariant.
        template<typename I, typename S /* = I */>
        struct sized_range
          : range<I, S>
        {
            iterator_size_t<I> third;

            sized_range() = default;
            constexpr sized_range(I begin, S end, iterator_size_t<I> size)
              : range<I, S>{detail::move(begin), detail::move(end)}, third(size)
            {}
            constexpr sized_range(std::pair<I, S> rng, iterator_size_t<I> size)
              : range<I, S>{detail::move(rng.first), detail::move(rng.second)}, third(size)
            {}
            constexpr sized_range(range<I, S> rng, iterator_size_t<I> size)
              : range<I, S>{detail::move(rng)}, third(size)
            {}
            iterator_size_t<I> size() const
            {
                RANGES_ASSERT(!ForwardIterator<I>() ||
                    static_cast<iterator_size_t<I>>(iterator_range_distance(this->first, this->second)) == third);
                return third;
            }
        };

        struct make_range_fn : bindable<make_range_fn>
        {
            template<typename I, typename S>
            static range<I, S> invoke(make_range_fn, I begin, S end)
            {
                CONCEPT_ASSERT(IteratorRange<I, S>());
                return {std::move(begin), std::move(end)};
            }

            template<typename I, typename S, typename Size>
            static sized_range<I, S> invoke(make_range_fn, I begin, S end, Size size)
            {
                CONCEPT_ASSERT(Integral<Size>());
                CONCEPT_ASSERT(IteratorRange<I, S>());
                return {std::move(begin), std::move(end), size};
            }
        };

        RANGES_CONSTEXPR make_range_fn make_range {};

        // TODO add specialization of is_infinite for when we can determine the range is infinite
    }
}

#endif
