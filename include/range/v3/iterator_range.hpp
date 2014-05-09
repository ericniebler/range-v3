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
#include <range/v3/utility/concepts.hpp>
#include <range/v3/utility/iterator_concepts.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace detail
        {
            template<typename I,
                typename Size = meta_apply<std::make_unsigned, iterator_difference_t<I>>,
                CONCEPT_REQUIRES_(RandomAccessIterator<I>())>
            Size
            iterator_range_size(I begin, I end)
            {
                return static_cast<Size>(end - begin);
            }

            template<typename I,
                typename Size = meta_apply<std::make_unsigned, iterator_difference_t<I>>>
            Size
            iterator_range_size(counted_iterator<I> begin, counted_sentinel<I> end)
            {
                return static_cast<Size>(end.count() - begin.count());
            }

            template<typename I,
                typename Size = meta_apply<std::make_unsigned, iterator_difference_t<I>>>
            Size
            iterator_range_size(counted_iterator<I> begin, counted_iterator<I> end)
            {
                RANGES_ASSERT(end.count() >= begin.count());
                return static_cast<Size>(end.count() - begin.count());
            }

            struct O1DistanceConcept
            {
                template<typename T, typename U>
                auto operator()(T t, U u) -> decltype(
                    concepts::valid_expr(
                        detail::iterator_range_size(t, u)
                    ));
            };

            template<typename T, typename U>
            using O1Distance = concepts::models<O1DistanceConcept, T, U>;
        }

        // Intentionally resisting the urge to fatten this interface to make
        // it look like a container, like boost::iterator_range. It's a range,
        // not a container.
        template<typename I, typename S /* = I */>
        struct iterator_range : private range_base
        {
            using size_type = meta_apply<std::make_unsigned, iterator_difference_t<I>>;
        private:
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
            CONCEPT_REQUIRES(detail::O1Distance<I, S>())
            size_type size() const
            {
                return detail::iterator_range_size(begin_end_.first(), begin_end_.second());
            }
            bool operator!() const
            {
                return begin() == end();
            }
            explicit operator bool() const
            {
                return !!*this;
            }
            iterator_range & advance_begin(iterator_difference_t<I> n)
            {
                advance(begin_end_.first(), n);
                return *this;
            }
            CONCEPT_REQUIRES(Same<I, S>())
            iterator_range & advance_end(iterator_difference_t<I> n)
            {
                advance(begin_end_.second(), n);
                return *this;
            }
        };

        template<typename I, typename S /* = I */>
        struct sized_iterator_range : private range_base
        {
            using size_type = meta_apply<std::make_unsigned, iterator_difference_t<I>>;
        private:
            compressed_tuple<I, S, size_type> begin_end_size_;
        public:
            using iterator = I;
            using sentinel = S;

            sized_iterator_range() = default;
            constexpr sized_iterator_range(I begin, S end, size_type size)
              : begin_end_size_(detail::move(begin), detail::move(end), size)
            {}
            constexpr sized_iterator_range(std::pair<I, S> rng, size_type size)
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
            size_type size() const
            {
                return ranges::get<2>(begin_end_size_);
            }
            bool operator!() const
            {
                return begin() == end();
            }
            explicit operator bool() const
            {
                return !!*this;
            }
            sized_iterator_range & advance_begin(iterator_difference_t<I> n)
            {
                advance(ranges::get<0>(begin_end_size_), n);
                return *this;
            }
            CONCEPT_REQUIRES(Same<I, S>())
            sized_iterator_range & advance_end(iterator_difference_t<I> n)
            {
                advance(ranges::get<1>(begin_end_size_), n);
                return *this;
            }
        };

        struct range_fn : bindable<range_fn>
        {
            template<typename I, typename S>
            static iterator_range<I, S> invoke(range_fn, I begin, S end)
            {
                CONCEPT_ASSERT(InputIterator<I>());
                CONCEPT_ASSERT(EqualityComparable<I, S>());
                return {std::move(begin), std::move(end)};
            }

            template<typename I, typename S, typename Size>
            static sized_iterator_range<I, S> invoke(range_fn, I begin, S end, Size size)
            {
                CONCEPT_ASSERT(Integral<Size>());
                CONCEPT_ASSERT(InputIterator<I>());
                CONCEPT_ASSERT(EqualityComparable<I, S>());
                return {std::move(begin), std::move(end), size};
            }
        };

        RANGES_CONSTEXPR range_fn range {};

        // TODO add specialization of is_infinite for when we can determine the range is infinite
    }
}

#endif
