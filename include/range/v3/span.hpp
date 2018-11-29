/// \file
// Range v3 library
//
//  Copyright Casey Carter 2016-2017
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
        /// \cond
        namespace detail
        {
            using span_index_t = std::ptrdiff_t;
        } // namespace detail
        /// \endcond

        constexpr detail::span_index_t dynamic_extent = -1;

        /// \cond
        namespace detail
        {
            template<typename To, typename From,
                CONCEPT_REQUIRES_(Integral<To>() && Integral<From>())>
            constexpr To narrow_cast(From from) noexcept
            {
                using C = common_type_t<To, From>;
                return RANGES_EXPECT((from > 0) == (static_cast<To>(from) > 0)),
                    RANGES_EXPECT(static_cast<C>(from) == static_cast<C>(static_cast<To>(from))),
                    static_cast<To>(from);
            }

            template<typename T>
            constexpr span_index_t byte_size(span_index_t n) noexcept
            {
                return n == dynamic_extent ? dynamic_extent
                    : (RANGES_EXPECT(n >= 0),
                       RANGES_EXPECT(narrow_cast<std::size_t>(n) <= PTRDIFF_MAX / sizeof(T)),
                       n * narrow_cast<span_index_t>(sizeof(T)));
            }

            template<span_index_t N>
            struct span_extent
            {
                CONCEPT_ASSERT(N >= 0);

                constexpr span_extent() noexcept = default;
                constexpr span_extent(span_index_t size) noexcept
                    // this constructor does nothing, the delegation exists only
                    // to provide a place for the contract check expression.
                  : span_extent{(RANGES_EXPECT(size == N), tag{})}
                {}

                constexpr span_index_t size() const noexcept
                {
                    return N;
                }
            private:
                struct tag {};
                constexpr span_extent(tag) noexcept
                {}
            };
            template<>
            struct span_extent<dynamic_extent>
            {
                span_extent() = default;
                constexpr span_extent(span_index_t size) noexcept
                  : size_{((void)RANGES_EXPECT(size >= 0), size)}
                {}
                constexpr span_index_t size() const noexcept
                {
                    return size_;
                }
            private:
                span_index_t size_ = 0;
            };

            constexpr span_index_t subspan_extent(
                span_index_t Extent, span_index_t Offset, span_index_t Count) noexcept
            {
                return Count == dynamic_extent && Extent != dynamic_extent
                    ? Extent - Offset : Count;
            }
        } // namespace detail
        /// \endcond

        template<typename T, detail::span_index_t N = dynamic_extent>
        struct RANGES_EMPTY_BASES span
          : public view_interface<span<T, N>,
                (N == dynamic_extent ? finite : static_cast<cardinality>(N))>,
            public detail::span_extent<N>
        {
            CONCEPT_ASSERT(std::is_object<T>::value);

            using element_type = T;
            using value_type = meta::_t<std::remove_cv<T>>;
            using index_type = detail::span_index_t;
            using difference_type = index_type;
            using pointer = T *;
            using reference = T &;
            using iterator = T *;
            using reverse_iterator = ranges::reverse_iterator<iterator>;

            static constexpr index_type extent = N;

            constexpr span() noexcept = default;
            constexpr span(pointer ptr, index_type count) noexcept
              : detail::span_extent<N>{(RANGES_EXPECT(count >= 0), count)}
              , data_{(RANGES_EXPECT(0 == count || ptr != nullptr), ptr)}
            {}
            template<typename = void> // Artificially templatize so that the other
                                      // constructor is preferred for {ptr, 0}
            constexpr span(pointer first, pointer last) noexcept
              : span{first, last - first}
            {}

            template<typename Rng>
            using CompatibleRange =
                meta::and_<meta::bool_<!Same<span, uncvref_t<Rng>>()>,
                    SizedRange<Rng>, ContiguousRange<Rng>,
                    detail::is_convertible<concepts::ContiguousRange::element_t<Rng>(*)[], T(*)[]>>;
            template<typename Rng>
            using DynamicConversion = meta::bool_<
                N == dynamic_extent || (range_cardinality<Rng>::value < cardinality())>;

            template<typename Rng,
                // This multiple-CONCEPT_REQUIRES_ form works around a gcc 4.9 bug.
                CONCEPT_REQUIRES_(CompatibleRange<Rng>()),
                CONCEPT_REQUIRES_(DynamicConversion<Rng>())>
            constexpr span(Rng &&rng)
                noexcept(noexcept(ranges::data(rng), ranges::size(rng)))
              : span{ranges::data(rng), detail::narrow_cast<index_type>(ranges::size(rng))}
            {}

            template<typename Rng>
            using StaticConversion = meta::bool_<
                N != dynamic_extent && range_cardinality<Rng>::value == N>;

            template<typename Rng,
                CONCEPT_REQUIRES_(CompatibleRange<Rng>()),
                CONCEPT_REQUIRES_(StaticConversion<Rng>())>
            constexpr span(Rng &&rng)
                noexcept(noexcept(ranges::data(rng)))
              : span{ranges::data(rng), N}
            {}

            template<index_type Count>
            constexpr span<T, Count> first() const noexcept
            {
                static_assert(Count >= 0,
                    "Count of elements to extract cannot be negative.");
                static_assert(N == dynamic_extent || Count <= N,
                    "Count of elements to extract must be less than the static span extent.");
                return RANGES_EXPECT(Count <= size()),
                    RANGES_EXPECT(Count == 0 || data_ != nullptr),
                    span<T, Count>{data_, Count};
            }
            constexpr span<T> first(index_type count) const noexcept
            {
                return RANGES_EXPECT(count >= 0 && count <= size()),
                    RANGES_EXPECT(count == 0 || data_ != nullptr),
                    span<T>{data_, count};
            }

            template<index_type Count>
            constexpr span<T, Count> last() const noexcept
            {
                static_assert(Count >= 0,
                    "Count of elements to extract cannot be negative.");
                static_assert(N == dynamic_extent || Count <= N,
                    "Count of elements to extract must be less than the static span extent.");
                return RANGES_EXPECT(Count <= size()),
                    RANGES_EXPECT((Count == 0 && size() == 0) || data_ != nullptr),
                    span<T, Count>{data_ + size() - Count, Count};
            }
            constexpr span<T> last(index_type count) const noexcept
            {
                return RANGES_EXPECT(count >= 0 && count <= size()),
                    RANGES_EXPECT((count == 0 && size() == 0) || data_ != nullptr),
                    span<T>{data_ + size() - count, count};
            }

            template<index_type Offset, index_type Count>
            constexpr span<T, detail::subspan_extent(N, Offset, Count)>
            subspan() const noexcept
            {
                static_assert(Offset >= 0,
                    "Offset of first element to extract cannot be negative.");
                static_assert(Count >= dynamic_extent,
                    "Count of elements to extract cannot be negative.");
                static_assert(N == dynamic_extent ||
                    N >= Offset + (Count == dynamic_extent ? 0 : Count),
                    "Sequence of elements to extract must be within the static span extent.");
                return RANGES_EXPECT(size() >= Offset + (Count == dynamic_extent ? 0 : Count)),
                    RANGES_EXPECT((Offset == 0 && Count <= 0) || data_ != nullptr),
                    span<T, detail::subspan_extent(N, Offset, Count)>{
                        data_ + Offset, Count == dynamic_extent ? size() - Offset : Count};
            }
            template<index_type Offset>
            constexpr span<T, (N >= Offset ? N - Offset : dynamic_extent)> subspan() const noexcept
            {
                static_assert(Offset >= 0,
                    "Offset of first element to extract cannot be negative.");
                static_assert(N == dynamic_extent || N >= Offset,
                    "Offset of first element to extract must be within the static span extent.");
                return RANGES_EXPECT(size() >= Offset),
                    RANGES_EXPECT((Offset == 0 && size() == 0) || data_ != nullptr),
                    span<T, N >= Offset ? N - Offset : dynamic_extent>{
                        data_ + Offset, size() - Offset};
            }
            constexpr span<T, dynamic_extent> subspan(index_type offset) const noexcept
            {
                return RANGES_EXPECT(offset >= 0),
                    RANGES_EXPECT(size() >= offset),
                    RANGES_EXPECT((offset == 0 && size() == 0) || data_ != nullptr),
                    span<T, dynamic_extent>{data_ + offset, size() - offset};
            }
            constexpr span<T, dynamic_extent> subspan(
                index_type offset, index_type count) const noexcept
            {
                return RANGES_EXPECT(offset >= 0),
                    RANGES_EXPECT(count >= 0),
                    RANGES_EXPECT(size() >= offset + count),
                    RANGES_EXPECT((offset == 0 && count == 0) || data_ != nullptr),
                    span<T, dynamic_extent>{data_ + offset, count};
            }

            constexpr pointer data() const noexcept { return data_; }
            using detail::span_extent<N>::size;
            constexpr index_type size_bytes() const noexcept
            {
                return detail::byte_size<T>(size());
            }
            constexpr bool empty() const noexcept { return size() == 0; }

            constexpr reference operator[](index_type idx) const noexcept
            {
                return RANGES_EXPECT(idx >= 0),
                    RANGES_EXPECT(idx < size()),
                    RANGES_EXPECT(data_),
                    data_[idx];
            }

            constexpr iterator begin() const noexcept
            {
                return RANGES_EXPECT(!size() || data_),
                    data_;
            }
            constexpr iterator end() const noexcept
            {
                return RANGES_EXPECT(!size() || data_),
                    data_ + size();
            }
            constexpr reverse_iterator rbegin() const noexcept { return reverse_iterator{end()}; }
            constexpr reverse_iterator rend() const noexcept { return reverse_iterator{begin()}; }

            template<typename U, index_type M,
                CONCEPT_REQUIRES_(EqualityComparable<T, U>())>
            bool operator==(span<U, M> const &that) const
            {
                RANGES_EXPECT(!size() || data());
                RANGES_EXPECT(!that.size() || that.data());
                return ranges::equal(*this, that);
            }
            template<typename U, index_type M,
                CONCEPT_REQUIRES_(EqualityComparable<T, U>())>
            bool operator!=(span<U, M> const &that) const
            {
                return !(*this == that);
            }

            template<typename U, index_type M,
                CONCEPT_REQUIRES_(TotallyOrdered<T, U>())>
            bool operator<(span<U, M> const &that) const
            {
                RANGES_EXPECT(!size() || data());
                RANGES_EXPECT(!that.size() || that.data());
                return ranges::lexicographical_compare(*this, that);
            }
            template<typename U, index_type M,
                CONCEPT_REQUIRES_(TotallyOrdered<T, U>())>
            bool operator>(span<U, M> const &that) const
            {
                return that < *this;
            }
            template<typename U, index_type M,
                CONCEPT_REQUIRES_(TotallyOrdered<T, U>())>
            bool operator<=(span<U, M> const &that) const
            {
                return !(that < *this);
            }
            template<typename U, index_type M,
                CONCEPT_REQUIRES_(TotallyOrdered<T, U>())>
            bool operator>=(span<U, M> const &that) const
            {
                return !(*this < that);
            }

        private:
            T *data_ = nullptr;
        };

        template<typename T, detail::span_index_t N>
        constexpr detail::span_index_t span<T, N>::extent;

#if RANGES_CXX_DEDUCTION_GUIDES >= RANGES_CXX_DEDUCTION_GUIDES_17
        template<typename Rng,
            CONCEPT_REQUIRES_(ContiguousRange<Rng>())>
        span(Rng &&rng) ->
            span<concepts::ContiguousRange::element_t<Rng>,
                (range_cardinality<Rng>::value < cardinality()
                    ? dynamic_extent
                    : static_cast<detail::span_index_t>(range_cardinality<Rng>::value))>;
#endif

        template<typename T, detail::span_index_t N>
        span<unsigned char const, detail::byte_size<T>(N)>
        as_bytes(span<T, N> s) noexcept
        {
            return {reinterpret_cast<unsigned char const *>(s.data()), s.size_bytes()};
        }
        template<typename T, detail::span_index_t N>
        span<unsigned char, detail::byte_size<T>(N)>
        as_writeable_bytes(span<T, N> s) noexcept
        {
            return {reinterpret_cast<unsigned char *>(s.data()), s.size_bytes()};
        }

        template<typename ElementType>
        constexpr span<ElementType>
        make_span(ElementType *ptr, detail::span_index_t count) noexcept
        {
            return span<ElementType>{ptr, count};
        }
        template<typename ElementType>
        constexpr span<ElementType>
        make_span(ElementType *first, ElementType *last) noexcept
        {
            return span<ElementType>{first, last};
        }
        template<typename Rng,
            CONCEPT_REQUIRES_(ContiguousRange<Rng>() &&
                range_cardinality<Rng>::value < cardinality())>
        constexpr span<concepts::ContiguousRange::element_t<Rng>>
        make_span(Rng &&rng)
            noexcept(noexcept(ranges::data(rng), ranges::size(rng)))
        {
            return {ranges::data(rng),
                detail::narrow_cast<detail::span_index_t>(ranges::size(rng))};
        }
        template<typename Rng,
            CONCEPT_REQUIRES_(ContiguousRange<Rng>() &&
                range_cardinality<Rng>::value >= cardinality())>
        constexpr span<concepts::ContiguousRange::element_t<Rng>,
            static_cast<detail::span_index_t>(range_cardinality<Rng>::value)>
        make_span(Rng &&rng)
            noexcept(noexcept(ranges::data(rng)))
        {
            return {ranges::data(rng), range_cardinality<Rng>::value};
        }
    } // namespace v3
} // namespace ranges

#endif // RANGES_V3_SPAN_HPP
