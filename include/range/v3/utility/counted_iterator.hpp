/// \file
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
#ifndef RANGES_V3_UTILITY_COUNTED_ITERATOR_HPP
#define RANGES_V3_UTILITY_COUNTED_ITERATOR_HPP

#include <utility>
#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/utility/iterator.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/basic_iterator.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \cond
        namespace detail
        {
            template<typename I, typename D /* = iterator_difference_t<I>*/>
            struct counted_cursor
            {
                using single_pass = SinglePass<I>;
                struct mixin
                  : basic_mixin<counted_cursor>
                {
                    RANGES_RELAXED_CONSTEXPR mixin() = default;
                    RANGES_RELAXED_CONSTEXPR mixin(counted_cursor pos)
                      : basic_mixin<counted_cursor>{std::move(pos)}
                    {}
                    RANGES_RELAXED_CONSTEXPR mixin(I it, D n)
                      : mixin(counted_cursor{it, n})
                    {}
                    RANGES_RELAXED_CONSTEXPR I base() const
                    {
                        return this->get().base();
                    }
                    RANGES_RELAXED_CONSTEXPR D count() const
                    {
                        return this->get().count();
                    }
                };
            private:
                friend struct counted_sentinel;
                using iterator_concept_ =
                    concepts::most_refined<meta::list<concepts::Iterator, concepts::WeakIterator>, I>;
                I it_;
                D n_;

                RANGES_RELAXED_CONSTEXPR
                bool equal_(counted_cursor const &that, concepts::WeakIterator*) const
                {
                    return n_ == that.n_;
                }
                RANGES_RELAXED_CONSTEXPR
                bool equal_(counted_cursor const &that, concepts::Iterator*) const
                {
                    return it_ == that.it_;
                }
                // Overload the advance algorithm for counted_iterators.
                // This is much faster. This gets found by ADL because
                // counted_cursor is an associated type of counted_iterator.
                RANGES_RELAXED_CONSTEXPR
                friend void advance(counted_iterator<I, D> &it, iterator_difference_t<I> n)
                {
                    counted_cursor &cur = get_cursor(it);
                    cur.n_ -= n;
                    ranges::advance(cur.it_, n);
                }
                // Overload uncounted and recounted for packing and unpacking
                // counted iterators
                RANGES_RELAXED_CONSTEXPR
                friend I uncounted(counted_iterator<I, D> i)
                {
                    return i.base();
                }
                RANGES_RELAXED_CONSTEXPR
                friend counted_iterator<I, D>
                recounted(counted_iterator<I, D> const &j, I i, iterator_difference_t<I> n)
                {
                    RANGES_ASSERT(!ForwardIterator<I>() || ranges::next(j.base(), n) == i);
                    return {i, j.count() - n};
                }
                CONCEPT_REQUIRES(RandomAccessIterator<I>())
                RANGES_RELAXED_CONSTEXPR
                friend counted_iterator<I, D> recounted(counted_iterator<I, D> const &j, I i)
                {
                    return {i, j.count() - (i - j.base())};
                }
            public:
                RANGES_RELAXED_CONSTEXPR counted_cursor() = default;
                RANGES_RELAXED_CONSTEXPR counted_cursor(I it, D n)
                  : it_(std::move(it)), n_(n)
                {}
                RANGES_RELAXED_CONSTEXPR
                auto current() const -> decltype(*it_)
                {
                    return *it_;
                }
                RANGES_RELAXED_CONSTEXPR void next()
                {
                    ++it_;
                    --n_;
                }
                CONCEPT_REQUIRES(EqualityComparable<D>() || Iterator<I>())
                RANGES_RELAXED_CONSTEXPR
                bool equal(counted_cursor const &that) const
                {
                    return this->equal_(that, iterator_concept_{});
                }
                CONCEPT_REQUIRES(BidirectionalIterator<I>())
                RANGES_RELAXED_CONSTEXPR void prev()
                {
                    --it_;
                    ++n_;
                }
                CONCEPT_REQUIRES(RandomAccessIterator<I>())
                RANGES_RELAXED_CONSTEXPR void advance(iterator_difference_t<I> n)
                {
                    it_ += n;
                    n_ -= n;
                }
                CONCEPT_REQUIRES(RandomAccessIterator<I>())
                RANGES_RELAXED_CONSTEXPR
                iterator_difference_t<I>
                distance_to(counted_cursor<I> const &that) const
                {
                    return that.it_ - it_;
                }
                I base() const
                {
                    return it_;
                }
                D count() const
                {
                    return n_;
                }
            };

            struct counted_sentinel
            {
                template<typename I, typename D>
                RANGES_RELAXED_CONSTEXPR
                bool equal(counted_cursor<I, D> const &that) const
                {
                    return that.n_ == 0;
                }
            };
        }
        /// \endcond

        /// \addtogroup group-utility
        /// @{

        // For RandomAccessIterator, operator- will be defined by basic_iterator
        template<typename I, typename D, CONCEPT_REQUIRES_(!RandomAccessIterator<I>())>
        RANGES_RELAXED_CONSTEXPR
        iterator_difference_t<I>
        operator-(counted_iterator<I, D> const &end, counted_iterator<I, D> const &begin)
        {
            return begin.count() - end.count();
        }

        template<typename I, typename D>
        RANGES_RELAXED_CONSTEXPR
        iterator_difference_t<I> operator-(counted_sentinel const &end, counted_iterator<I, D> const &begin)
        {
            return begin.count();
        }

        template<typename I, typename D>
        RANGES_RELAXED_CONSTEXPR
        iterator_difference_t<I> operator-(counted_iterator<I, D> const &begin, counted_sentinel const &end)
        {
            return -begin.count();
        }

        template<typename I>
        RANGES_RELAXED_CONSTEXPR
        iterator_difference_t<I> operator-(counted_sentinel const &, counted_sentinel const &)
        {
            return 0;
        }
        /// @}
    }
}

#endif
