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
        // Like boost::iterator_range.
        template<typename I, typename S /* = I */>
        struct range
          : private compressed_pair<I, S>
        {
            using iterator = I;
            using sentinel = S;
            using compressed_pair<I, S>::first;
            using compressed_pair<I, S>::second;

            range() = default;
            constexpr range(I begin, S end)
              : compressed_pair<I, S>{detail::move(begin), detail::move(end)}
            {}
            constexpr range(std::pair<I, S> rng)
              : compressed_pair<I, S>{detail::move(rng.first), detail::move(rng.second)}
            {}
            I begin() const
            {
                return first;
            }
            S end() const
            {
                return second;
            }
            bool empty() const
            {
                return first == second;
            }
            bool operator!() const
            {
                return empty();
            }
            explicit operator bool() const
            {
                return !empty();
            }
            CONCEPT_REQUIRES(SizedIteratorRange<I, S>())
            iterator_size_t<I> size() const
            {
                return iterator_range_size(first, second);
            }
            template<typename X, typename Y,
                CONCEPT_REQUIRES_(Constructible<X, I const &>() &&
                                  Constructible<Y, S const &>())>
            constexpr operator std::pair<X, Y>() const
            {
                return std::pair<X, Y>{first, second};
            }
            iterator_reference_t<I> front() const
            {
                RANGES_ASSERT(!empty());
                return *first;
            }
            void pop_front()
            {
                ++first;
            }
            CONCEPT_REQUIRES(BidirectionalIterator<S>())
            iterator_reference_t<I> back() const
            {
                RANGES_ASSERT(!empty());
                return *prev(second);
            }
            CONCEPT_REQUIRES(BidirectionalIterator<S>())
            void pop_back()
            {
                --second;
            }
            // The requirements on the return type of I::operator[] are different
            // than for I::operator*, so respect that here.
            template<typename F = I, CONCEPT_REQUIRES_(RandomAccessIterator<I>())>
            auto operator[](iterator_difference_t<I> n) const ->
                decltype(std::declval<F>()[n])
            {
                return first[n];
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
              : range<I, S>{detail::move(rng)}, third(size)
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

            // It's just too easy to use these without knowing you're invaliding
            // the size() function.
            void pop_front() = delete;
            void pop_back() = delete;
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

        // Tuple-like access
        template<std::size_t I, typename First, typename Second>
        constexpr auto get(range<First, Second> & p) ->
            typename std::enable_if<I == 0, decltype((p.first))>::type
        {
            return p.first;
        }

        template<std::size_t I, typename First, typename Second>
        constexpr auto get(range<First, Second> const & p) ->
            typename std::enable_if<I == 0, decltype((p.first))>::type
        {
            return p.first;
        }

        template<std::size_t I, typename First, typename Second>
        constexpr auto get(range<First, Second> && p) ->
            typename std::enable_if<I == 0, decltype((detail::move(p).first))>::type
        {
            return detail::move(p).first;
        }

        template<std::size_t I, typename First, typename Second>
        constexpr auto get(range<First, Second> & p) ->
            typename std::enable_if<I == 1, decltype((p.second))>::type
        {
            return p.second;
        }

        template<std::size_t I, typename First, typename Second>
        constexpr auto get(range<First, Second> const & p) ->
            typename std::enable_if<I == 1, decltype((p.second))>::type
        {
            return p.second;
        }

        template<std::size_t I, typename First, typename Second>
        constexpr auto get(range<First, Second> && p) ->
            typename std::enable_if<I == 1, decltype((detail::move(p).second))>::type
        {
            return detail::move(p).second;
        }

        // TODO add specialization of is_infinite for when we can determine the range is infinite
    }
}

// The standard is inconsistent about whether these are classes or structs
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Wmismatched-tags"

namespace std
{
    template<typename First, typename Second>
    struct tuple_size< ::ranges::v3::range<First, Second>>
      : std::integral_constant<size_t, 2>
    {};

    template<typename First, typename Second>
    struct tuple_element<0, ::ranges::v3::range<First, Second>>
    {
        using type = First;
    };

    template<typename First, typename Second>
    struct tuple_element<1, ::ranges::v3::range<First, Second>>
    {
        using type = Second;
    };

    template<typename First, typename Second>
    struct tuple_size< ::ranges::v3::sized_range<First, Second>>
      : std::integral_constant<size_t, 2>
    {};

    template<typename First, typename Second>
    struct tuple_element<0, ::ranges::v3::sized_range<First, Second>>
    {
        using type = First;
    };

    template<typename First, typename Second>
    struct tuple_element<1, ::ranges::v3::sized_range<First, Second>>
    {
        using type = Second;
    };
}

#pragma GCC diagnostic pop

#endif
