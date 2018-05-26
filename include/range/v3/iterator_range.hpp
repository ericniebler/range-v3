/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2013-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGES_V3_ITERATOR_RANGE_HPP
#define RANGES_V3_ITERATOR_RANGE_HPP

#include <utility>
#include <type_traits>
#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/distance.hpp>
#include <range/v3/view_interface.hpp>
#include <range/v3/utility/concepts.hpp>
#include <range/v3/utility/iterator.hpp>
#include <range/v3/utility/tagged_pair.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/algorithm/tagspec.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-core
        /// @{
        template<typename I, typename S /*= I*/>
        struct RANGES_EMPTY_BASES iterator_range
          : tagged_compressed_tuple<tag::begin(I), tag::end(S)>
          , view_interface<iterator_range<I, S>>
        {
        private:
            using base_t = tagged_compressed_tuple<tag::begin(I), tag::end(S)>;
        public:
            using iterator = I;
            using sentinel = S;
        #ifndef RANGES_DOXYGEN_INVOKED
            using const_iterator = I; // Mostly to avoid spurious errors in Boost.Range
        #endif

            using base_t::begin;
            using base_t::end;

            iterator_range() = default;
            constexpr iterator_range(I begin, S end)
              : base_t{detail::move(begin), detail::move(end)}
            {}
            template<typename X, typename Y,
                CONCEPT_REQUIRES_(Constructible<I, X>() && Constructible<S, Y>())>
            constexpr iterator_range(iterator_range<X, Y> rng)
              : base_t{detail::move(rng.begin()), detail::move(rng.end())}
            {}
            template<typename X, typename Y,
                CONCEPT_REQUIRES_(Constructible<I, X>() && Constructible<S, Y>())>
            explicit constexpr iterator_range(std::pair<X, Y> rng)
              : base_t{detail::move(rng.first), detail::move(rng.second)}
            {}
            template<typename X, typename Y,
                CONCEPT_REQUIRES_(Assignable<I &, X>() && Assignable<S &, Y>())>
            iterator_range &operator=(iterator_range<X, Y> rng)
            {
                begin() = detail::move(rng).begin();
                end() = detail::move(rng).end();
                return *this;
            }
            template<typename X, typename Y,
                CONCEPT_REQUIRES_(ConvertibleTo<I, X>() && ConvertibleTo<S, Y>())>
            constexpr operator std::pair<X, Y>() const
            {
                return {begin(), end()};
            }
            constexpr bool empty() const
            {
                return begin() == end();
            }
        };

        // Like iterator_range, but with a known size. The first and second members
        // are private to prevent inadvertent violations of the class invariant.
        //
        // Class invariant:
        //   distance(begin(), end()) == size()
        //
        template<typename I, typename S /* = I */>
        struct sized_iterator_range
          : view_interface<sized_iterator_range<I, S>>
        {
        private:
            template<typename X, typename Y>
            friend struct sized_iterator_range;
            iterator_range<I, S> rng_;
            size_type_t<I> size_;
        public:
            using iterator = I;
            using sentinel = S;
        #ifndef RANGES_DOXYGEN_INVOKED
            using const_iterator = I; // Mostly to avoid spurious errors in Boost.Range
        #endif

            sized_iterator_range() = default;
            RANGES_NDEBUG_CONSTEXPR sized_iterator_range(I begin, S end, size_type_t<I> size)
              : rng_{detail::move(begin), detail::move(end)}, size_(size)
            {
            #ifndef NDEBUG
                RANGES_ASSERT(!ForwardIterator<I>() ||
                    static_cast<size_type_t<I>>(ranges::distance(rng_)) == size_);
            #endif
            }
            template<typename X, typename Y,
                CONCEPT_REQUIRES_(Constructible<I, X>() && Constructible<S, Y>())>
            RANGES_NDEBUG_CONSTEXPR sized_iterator_range(std::pair<X, Y> rng, size_type_t<I> size)
              : sized_iterator_range{detail::move(rng).first, detail::move(rng).second, size}
            {}
            template<typename X, typename Y,
                CONCEPT_REQUIRES_(Constructible<I, X>() && Constructible<S, Y>())>
            RANGES_NDEBUG_CONSTEXPR sized_iterator_range(iterator_range<X, Y> rng, size_type_t<I> size)
              : sized_iterator_range{detail::move(rng).first(), detail::move(rng).second, size}
            {}
            template<typename X, typename Y,
                CONCEPT_REQUIRES_(Constructible<I, X>() && Constructible<S, Y>())>
            RANGES_NDEBUG_CONSTEXPR sized_iterator_range(sized_iterator_range<X, Y> rng)
              : sized_iterator_range{detail::move(rng).rng_.first(), detail::move(rng).rng_.second, rng.size_}
            {}
            template<typename X, typename Y,
                CONCEPT_REQUIRES_(Assignable<I &, X>() && Assignable<S &, Y>())>
            sized_iterator_range &operator=(sized_iterator_range<X, Y> rng)
            {
                rng_ = detail::move(rng).rng_;
                size_ = rng.size_;
                return *this;
            }
            I begin() const
            {
                return rng_.begin();
            }
            S end() const
            {
                return rng_.end();
            }
            size_type_t<I> size() const
            {
                return size_;
            }
            template<typename X, typename Y,
                CONCEPT_REQUIRES_(ConvertibleTo<I, X>() && ConvertibleTo<S, Y>())>
            constexpr operator std::pair<X, Y>() const
            {
                return rng_;
            }
            template<typename X, typename Y,
                CONCEPT_REQUIRES_(ConvertibleTo<I, X>() && ConvertibleTo<S, Y>())>
            constexpr operator iterator_range<X, Y>() const
            {
                return rng_;
            }
            constexpr operator iterator_range<I, S> const &() const & noexcept
            {
                return rng_;
            }
        };

        struct make_iterator_range_fn
        {
            /// \return `{begin, end}`
            template<typename I, typename S,
                CONCEPT_REQUIRES_(Sentinel<S, I>())>
            constexpr iterator_range<I, S> operator()(I begin, S end) const
            {
                CONCEPT_ASSERT(Sentinel<S, I>());
                return {detail::move(begin), detail::move(end)};
            }

            /// \return `{begin, end, size}`
            template<typename I, typename S,
                CONCEPT_REQUIRES_(Sentinel<S, I>())>
            constexpr sized_iterator_range<I, S> operator()(I begin, S end, size_type_t<I> size) const
            {
                CONCEPT_ASSERT(Sentinel<S, I>());
                return {detail::move(begin), detail::move(end), size};
            }
        };

        /// \ingroup group-core
        /// \sa `make_iterator_range_fn`
        RANGES_INLINE_VARIABLE(make_iterator_range_fn, make_iterator_range)

        /// Tuple-like access for `sized_iterator_range`
        template<std::size_t N, typename I, typename S,
            CONCEPT_REQUIRES_(N < 2)>
        constexpr auto get(sized_iterator_range<I, S> const &p)
        RANGES_DECLTYPE_AUTO_RETURN
        (
            ranges::get<N>(static_cast<iterator_range<I, S> const &>(p))
        )

        /// \overload
        template<std::size_t N, typename I, typename S,
            CONCEPT_REQUIRES_(N == 2)>
        constexpr size_type_t<I> get(sized_iterator_range<I, S> const &p)
        {
            return p.size();
        }


        // TODO add specialization of range_cardinality for when we can determine the range is infinite

        /// @}
    }
}

// The standard is inconsistent about whether these are classes or structs
RANGES_DIAGNOSTIC_PUSH
RANGES_DIAGNOSTIC_IGNORE_MISMATCHED_TAGS

/// \cond
namespace std
{
    template<typename I, typename S>
    struct tuple_size< ::ranges::v3::iterator_range<I, S>>
      : std::integral_constant<size_t, 2>
    {};

    template<typename I, typename S>
    struct tuple_element<0, ::ranges::v3::iterator_range<I, S>>
    {
        using type = I;
    };

    template<typename I, typename S>
    struct tuple_element<1, ::ranges::v3::iterator_range<I, S>>
    {
        using type = S;
    };

    template<typename I, typename S>
    struct tuple_size< ::ranges::v3::sized_iterator_range<I, S>>
      : std::integral_constant<size_t, 3>
    {};

    template<typename I, typename S>
    struct tuple_element<0, ::ranges::v3::sized_iterator_range<I, S>>
    {
        using type = I;
    };

    template<typename I, typename S>
    struct tuple_element<1, ::ranges::v3::sized_iterator_range<I, S>>
    {
        using type = S;
    };

    template<typename I, typename S>
    struct tuple_element<2, ::ranges::v3::sized_iterator_range<I, S>>
    {
        using type = ::ranges::v3::size_type_t<I>;
    };
}
/// \endcond

RANGES_DIAGNOSTIC_POP

#endif
