/// \file
// Range v3 library
//
//  Copyright Casey Carter 2016
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGES_V3_SPAN_HPP
#define RANGES_V3_SPAN_HPP

#include <cstddef>
#include <meta/meta.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/data.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/size.hpp>
#include <range/v3/view_interface.hpp>
#include <range/v3/algorithm/equal.hpp>
#include <range/v3/algorithm/lexicographical_compare.hpp>
#include <range/v3/utility/iterator.hpp>

namespace ranges
{
    inline namespace v3
    {
        constexpr std::ptrdiff_t dynamic_extent = -1;

        /// \cond
        namespace detail
        {
            // true iff T is U, possibly with more cv-qualifiers
            template<typename T, typename U>
            using is_more_cv_than = meta::strict_and<
                std::is_same<meta::_t<std::remove_cv<T>>, meta::_t<std::remove_cv<U>>>,
                std::is_convertible<U*, T*>>;

            template<typename T>
            constexpr std::ptrdiff_t byte_size(std::ptrdiff_t n) noexcept
            {
                return n == dynamic_extent ? dynamic_extent
                    : n * static_cast<std::ptrdiff_t>(sizeof(T));
            }

            template<typename T, std::ptrdiff_t N>
            class span_base
            {
            public:
                CONCEPT_ASSERT(0 <= N);

                span_base() = default;
                constexpr span_base(T* ptr, std::ptrdiff_t size) noexcept
                  : ptr_{(RANGES_EXPECT(ptr || !N), RANGES_EXPECT(size == N), ptr)}
                {}
                constexpr T* data() const noexcept
                {
                    return ptr_;
                }
                constexpr std::ptrdiff_t size() const noexcept
                {
                    return N;
                }
            protected:
                T* ptr_ = nullptr;
            };

            template<typename T>
            class span_base<T, dynamic_extent>
            {
            public:
                span_base() = default;
                constexpr span_base(T* ptr, std::ptrdiff_t size) noexcept
                  : ptr_{(RANGES_EXPECT(ptr || !size), ptr)}
                  , size_{(RANGES_EXPECT(0 <= size), size)}
                {}
                constexpr T* data() const noexcept
                {
                    return ptr_;
                }
                constexpr std::ptrdiff_t size() const noexcept
                {
                    return size_;
                }
            protected:
                T* ptr_ = nullptr;
                std::ptrdiff_t size_ = 0;
            };
        } // namespace detail
        /// \endcond

        template<typename T, std::ptrdiff_t N = dynamic_extent>
        class span
          : public view_interface<span<T, N>,
                (N == dynamic_extent ? finite : static_cast<cardinality>(N))>
        {
            CONCEPT_ASSERT(N == dynamic_extent || 0 <= N);

            detail::span_base<T, N> storage_ = {};

        public:
            using element_type = T;
            using difference_type = std::ptrdiff_t;
            using index_type = std::ptrdiff_t;
            using pointer = T*;
            using reference = T&;
            using iterator = T*;
            using reverse_iterator = ranges::reverse_iterator<iterator>;

            static constexpr index_type extent = N;

            // [span.cons], span constructors, copy, assignment, and destructor
            constexpr span() noexcept = default;
            constexpr span(std::nullptr_t) noexcept
              : span{}
            {}
            constexpr span(pointer ptr, index_type count) noexcept
              : storage_{ptr, count}
            {}
            constexpr span(pointer first, pointer last) noexcept
              : span{first, last - first}
            {}
            // EXTENSION: disambiguate initialization from "pointer, small-literal"
            template<typename Int,
                CONCEPT_REQUIRES_(SignedIntegral<Int>())>
            constexpr span(pointer first, Int count) noexcept
              : span{first, index_type{count}}
            {}

            template<typename Rng>
            using ConversionConstraint =
                meta::bool_<SizedRange<Rng>() && ContiguousRange<Rng>() &&
                    detail::is_more_cv_than<T, concepts::ContiguousRange::datum_t<Rng>>() &&
                    (N == dynamic_extent ||
                        range_cardinality<Rng>::value == finite ||
                        range_cardinality<Rng>::value == N)>;

            template<typename Rng,
                CONCEPT_REQUIRES_(ConversionConstraint<Rng>())>
            constexpr span(Rng && rng)
              : span{ranges::data(rng), static_cast<index_type>(ranges::size(rng))}
            {}

            // [span.sub], span subviews
            template<std::ptrdiff_t Count,
                CONCEPT_REQUIRES_(N == dynamic_extent || Count <= N)>
            constexpr span<T, Count> first() const noexcept
            {
                static_assert(0 <= Count,
                    "Count of characters to extract cannot be negative.");
                return RANGES_EXPECT(Count <= size()),
                    span<T, Count>{data(), Count};
            }
            template<std::ptrdiff_t Count,
                CONCEPT_REQUIRES_(N == dynamic_extent || Count <= N)>
            constexpr span<T, Count> last() const noexcept
            {
                static_assert(0 <= Count,
                    "Count of characters to extract cannot be negative.");
                return RANGES_EXPECT(Count <= size()),
                    span<T, Count>{data() + size() - Count, Count};
            }
            template<std::ptrdiff_t Offset, std::ptrdiff_t Count = dynamic_extent,
                CONCEPT_REQUIRES_(Count == dynamic_extent ||
                    (0 <= Count && (N == dynamic_extent || Offset + Count <= N)))>
            constexpr span<T, Count> subspan() const noexcept
            {
                static_assert(0 <= Offset,
                    "Offset for subspan cannot be negative.");
                return RANGES_EXPECT(Offset <= size()),
                    RANGES_EXPECT(Count == dynamic_extent || Offset + Count <= size()),
                    span<T, Count>{data() + Offset,
                        Count == dynamic_extent ? size() - Offset : Count};
            }
            constexpr span<T> first(index_type count) const noexcept
            {
                return RANGES_EXPECT(0 <= count && count <= size()),
                    span<T>{data(), count};
            }
            constexpr span<T> last(index_type count) const noexcept
            {
                return RANGES_EXPECT(0 <= count && count <= size()),
                    span<T>{data() + size() - count, count};
            }
            constexpr span<T> subspan(
                index_type offset, index_type count = dynamic_extent) const noexcept
            {
                return RANGES_EXPECT(0 <= offset && offset <= size()),
                    RANGES_EXPECT(count == dynamic_extent ||
                        (0 <= count && offset + count <= size())),
                    span<T>{data() + offset,
                        count == dynamic_extent ? size() - offset : count};
            }

            // [span.obs], span observers
            constexpr pointer data() const noexcept { return storage_.data(); }
            constexpr index_type size() const noexcept { return storage_.size(); }
            constexpr index_type length() const noexcept { return size(); }
            constexpr index_type length_bytes() const noexcept { return size_bytes(); }
            constexpr index_type size_bytes() const noexcept
            {
                return detail::byte_size<T>(size());
            }
            constexpr bool empty() const noexcept { return size() == 0; }

            // [span.elem], span element access
            constexpr reference operator[](index_type idx) const noexcept
            {
                return RANGES_EXPECT(0 <= idx && idx < size()),
                    RANGES_EXPECT(data()),
                    *(data() + idx);
            }
            constexpr reference operator()(index_type idx) const noexcept
            {
                return (*this)[idx];
            }

            // [span.iter], span iterator support
            iterator begin() const noexcept { return data(); }
            iterator end() const noexcept { return data() + size(); }
            reverse_iterator rbegin() const noexcept { return reverse_iterator{end()}; }
            reverse_iterator rend() const noexcept { return reverse_iterator{begin()}; }
        };

        template<typename T, std::ptrdiff_t N>
        constexpr std::ptrdiff_t span<T, N>::extent;

        // [span.comparison], span comparison operators
        template<class T, class U, std::ptrdiff_t N,
            CONCEPT_REQUIRES_(EqualityComparable<T, U>())>
        constexpr bool operator==(
            span<T, N> const &l, span<U, N> const &r)
        {
            return ranges::equal(l, r);
        }
        template<class T, class U, std::ptrdiff_t N,
            CONCEPT_REQUIRES_(EqualityComparable<T, U>())>
        constexpr bool operator!=(
            span<T, N> const &l, span<U, N> const &r)
        {
            return !(l == r);
        }

        template<class T, class U, std::ptrdiff_t N,
            CONCEPT_REQUIRES_(TotallyOrdered<T, U>())>
        constexpr bool operator<(
            span<T, N> const &l, span<U, N> const &r)
        {
            return ranges::lexicographical_compare(l, r);
        }
        template<class T, class U, std::ptrdiff_t N,
            CONCEPT_REQUIRES_(TotallyOrdered<T, U>())>
        constexpr bool operator<=(
            span<T, N> const &l, span<U, N> const &r)
        {
            return !(r < l);
        }
        template<class T, class U, std::ptrdiff_t N,
            CONCEPT_REQUIRES_(TotallyOrdered<T, U>())>
        constexpr bool operator>(
            span<T, N> const &l, span<U, N> const &r)
        {
            return r < l;
        }
        template<class T, class U, std::ptrdiff_t N,
            CONCEPT_REQUIRES_(TotallyOrdered<T, U>())>
        constexpr bool operator>=(
            span<T, N> const &l, span<U, N> const &r)
        {
            return !(l < r);
        }

        // [span.objectrep], views of object representation
        template<class T, std::ptrdiff_t N>
        span<unsigned char const, detail::byte_size<T>(N)> as_bytes(span<T, N> s) noexcept
        {
            return {reinterpret_cast<unsigned char const *>(s.data()), s.size_bytes()};
        }
        template<class T, std::ptrdiff_t N>
        span<unsigned char, detail::byte_size<T>(N)> as_writeable_bytes(span<T, N> s) noexcept
        {
            return {reinterpret_cast<unsigned char *>(s.data()), s.size_bytes()};
        }
    } // namespace v3
} // namespace ranges

#endif // RANGES_V3_SPAN_HPP
