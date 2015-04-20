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
// The code below is adapted from libc++'s std::array
// which is dual licensed under the MIT and the University of Illinois Open
// Source Licenses.
//
// See http://libcxx.llvm.com
//
#ifndef RANGES_V3_UTILITY_REVERSE_ITERATOR_HPP
#define RANGES_V3_UTILITY_REVERSE_ITERATOR_HPP

#include <range/v3/utility/addressof.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>

namespace ranges {
    inline namespace v3 {

        template <typename It>
        class reverse_iterator {
            static_assert(Iterator<It>{},
                          "reverse_iterator requires a model of Iterator");
            static_assert(BidirectionalIterator<It>{},
                          "reverse_iterator requires a model of BidirectionalIterator");

          public:
            using iterator_category = iterator_category_t<It>;
            using value_type = iterator_value_t<It>;
            using difference_type = iterator_difference_t<It>;
            using pointer = typename std::iterator_traits<It>::pointer;
            using reference = iterator_reference_t<It>;
            using iterator_type = It;
          protected:
            It current;
          public:
            RANGES_RELAXED_CONSTEXPR reverse_iterator() : current() {}
            RANGES_RELAXED_CONSTEXPR
            explicit reverse_iterator(iterator_type other)
              : current(std::move(other))
            {}
            template <typename U>
            RANGES_RELAXED_CONSTEXPR
            reverse_iterator(reverse_iterator<U> const& u)
              : current(u.base())
            {}
            RANGES_RELAXED_CONSTEXPR iterator_type base() const
            {
                return current;
            }
            RANGES_RELAXED_CONSTEXPR reference operator*() const
            {
                iterator_type tmp(current);
                return *--tmp;
           }
           RANGES_RELAXED_CONSTEXPR pointer operator->() const
           {
                return ranges::addressof(operator*());
           }
           RANGES_RELAXED_CONSTEXPR reverse_iterator& operator++()
           {
               --current;
               return *this;
           }
           RANGES_RELAXED_CONSTEXPR reverse_iterator operator++(int)
           {
               reverse_iterator tmp(*this);
               --current;
               return tmp;
           }
           RANGES_RELAXED_CONSTEXPR reverse_iterator& operator--()
           {
               ++current;
               return *this;
           }
           RANGES_RELAXED_CONSTEXPR reverse_iterator operator--(int)
           {
               reverse_iterator tmp(*this);
               ++current;
               return tmp;
           }
           CONCEPT_REQUIRES(RandomAccessIterator<iterator_type>{})
           RANGES_RELAXED_CONSTEXPR
           reverse_iterator operator+ (difference_type n) const
           {
               return reverse_iterator(current - n);
           }
           CONCEPT_REQUIRES(RandomAccessIterator<iterator_type>{})
           RANGES_RELAXED_CONSTEXPR
           reverse_iterator& operator+=(difference_type n)
           {
               current -= n;
               return *this;
           }
           CONCEPT_REQUIRES(RandomAccessIterator<iterator_type>{})
           RANGES_RELAXED_CONSTEXPR
           reverse_iterator operator-(difference_type n) const {
               return reverse_iterator(current + n);
           }
           RANGES_RELAXED_CONSTEXPR reverse_iterator& operator-=(difference_type n)
           {
               current += n;
               return *this;
           }
           CONCEPT_REQUIRES(RandomAccessIterator<iterator_type>{})
           RANGES_RELAXED_CONSTEXPR reference operator[](difference_type n) const
           {
               return *(*this + n);
           }
        };

        template <typename It1, typename It2>
        RANGES_RELAXED_CONSTEXPR
        bool
        operator==(const reverse_iterator<It1>& x, const reverse_iterator<It2>& y)
        {
            return x.base() == y.base();
        }

        template <typename It1, typename It2>
        RANGES_RELAXED_CONSTEXPR
        bool
        operator<(const reverse_iterator<It1>& x, const reverse_iterator<It2>& y)
        {
            return x.base() > y.base();
        }

        template <typename It1, typename It2>
        RANGES_RELAXED_CONSTEXPR
        bool
        operator!=(const reverse_iterator<It1>& x, const reverse_iterator<It2>& y)
        {
            return x.base() != y.base();
        }

        template <typename It1, typename It2>
        RANGES_RELAXED_CONSTEXPR
        bool
        operator>(const reverse_iterator<It1>& x, const reverse_iterator<It2>& y)
        {
            return x.base() < y.base();
        }

        template <typename It1, typename It2>
        RANGES_RELAXED_CONSTEXPR
        bool
        operator>=(const reverse_iterator<It1>& x, const reverse_iterator<It2>& y)
        {
            return x.base() <= y.base();
        }

        template <typename It1, typename It2>
        RANGES_RELAXED_CONSTEXPR
        bool
        operator<=(const reverse_iterator<It1>& x, const reverse_iterator<It2>& y)
        {
            return x.base() >= y.base();
        }

        template <typename It1, typename It2,
                  CONCEPT_REQUIRES_(RandomAccessIterator<It1>{} &&
                                    RandomAccessIterator<It2>{})>
        RANGES_RELAXED_CONSTEXPR
        iterator_difference_t<It1>
        operator-(const reverse_iterator<It1>& x, const reverse_iterator<It2>& y)
        {
            return y.base() - x.base();
        }

        template <typename It, CONCEPT_REQUIRES_(RandomAccessIterator<It>{})>
        RANGES_RELAXED_CONSTEXPR
        reverse_iterator<It>
        operator+(iterator_difference_t<reverse_iterator<It>> n, const reverse_iterator<It>& x)
        {
            return reverse_iterator<It>(x.base() - n);
        }

        template <typename It, CONCEPT_REQUIRES_(BidirectionalIterator<It>{})>
        RANGES_RELAXED_CONSTEXPR
        reverse_iterator<It> make_reverse_iterator(It i)
        {
            return reverse_iterator<It>(i);
        }

    }  // namespace v3
}  // namespace ranges
#endif  // RANGES_V3_UTILITY_REVERSE_ITERATOR_HPP
