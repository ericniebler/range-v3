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
#include <type_traits>
#include <range/v3/range_fwd.hpp>
#include <range/v3/utility/meta.hpp>
#include <range/v3/utility/bindable.hpp>
#include <range/v3/utility/iterator.hpp>
#include <range/v3/utility/compressed_pair.hpp>
#include <range/v3/utility/compressed_tuple.hpp>
#include <range/v3/utility/iterator_concepts.hpp>

namespace ranges
{
    inline namespace v3
    {
        // Intentionally resisting the urge to fatten this interface to make
        // it look like a container, like boost::iterator_range. It's a range,
        // not a container.
        template<typename I, typename S /* = I */>
        struct iterator_range : private range_base
        {
            compressed_pair<I, S> begin_end_;
        public:
            using iterator = I;
            using sentinel = S;

            iterator_range() = default;
            constexpr iterator_range(I begin, S end)
              : begin_end_(detail::move(begin), detail::move(end))
            {}
            constexpr iterator_range(std::pair<I, S> rng)
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
            CONCEPT_REQUIRES(SizedIteratorRange<I, S>())
            iterator_size_t<I> size() const
            {
                return iterator_range_size(begin_end_.first(), begin_end_.second());
            }
            bool operator!() const
            {
                return begin() == end();
            }
            explicit operator bool() const
            {
                return !!*this;
            }
        };

        template<typename I, typename S /* = I */>
        struct sized_iterator_range : private range_base
        {
        private:
            compressed_tuple<I, S, iterator_size_t<I>> begin_end_size_;
        public:
            using iterator = I;
            using sentinel = S;

            sized_iterator_range() = default;
            constexpr sized_iterator_range(I begin, S end, iterator_size_t<I> size)
              : begin_end_size_(detail::move(begin), detail::move(end), size)
            {
                #if __cplusplus > 201103L
                RANGES_ASSERT(iterator_range_size(this->begin(), this->end()) == size);
                #endif
            }
            constexpr sized_iterator_range(std::pair<I, S> rng, iterator_size_t<I> size)
              : begin_end_size_(detail::move(rng.first), detail::move(rng.second), size)
            {}
            iterator begin() const
            {
                return ranges::get<0>(begin_end_size_);
            }
            sentinel end() const
            {
                return ranges::get<1>(begin_end_size_);
            }
            iterator_size_t<I> size() const
            {
                return ranges::get<2>(begin_end_size_);
            }
            bool operator!() const
            {
                return 0 == size();
            }
            explicit operator bool() const
            {
                return !!*this;
            }
        };

        struct range_fn : bindable<range_fn>
        {
            template<typename I, typename S>
            static iterator_range<I, S> invoke(range_fn, I begin, S end)
            {
                CONCEPT_ASSERT(IteratorRange<I, S>());
                return {std::move(begin), std::move(end)};
            }

            template<typename I, typename S, typename Size>
            static sized_iterator_range<I, S> invoke(range_fn, I begin, S end, Size size)
            {
                CONCEPT_ASSERT(Integral<Size>());
                CONCEPT_ASSERT(IteratorRange<I, S>());
                return {std::move(begin), std::move(end), size};
            }
        };

        RANGES_CONSTEXPR range_fn range {};

        // TODO add specialization of is_infinite for when we can determine the range is infinite
    }
}

#endif
