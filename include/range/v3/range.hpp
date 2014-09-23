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
#include <range/v3/utility/meta.hpp>
#include <range/v3/utility/bindable.hpp>
#include <range/v3/utility/iterator.hpp>
#include <range/v3/utility/compressed_pair.hpp>
#include <range/v3/utility/iterator_concepts.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace detail
        {
            template<typename T, typename U = meta_apply<std::remove_const, T>>
            constexpr U && unsafe_move(T &t)
            {
                return static_cast<U &&>(const_cast<U &>(t));
            }
        }

        // Like boost::iterator_range.
        template<typename I, typename S /*= I*/>
        struct range
          : private compressed_pair<I, S>
        {
            using iterator = meta_apply<std::remove_const, I>;
            using sentinel = meta_apply<std::remove_const, S>;
            using compressed_pair<I, S>::first;
            using compressed_pair<I, S>::second;

            range() = default;
            constexpr range(I begin, S end)
              : compressed_pair<I, S>{detail::move(begin), detail::move(end)}
            {}
            template<typename X, typename Y,
                CONCEPT_REQUIRES_(Constructible<I, X &&>() && Constructible<S, Y &&>())>
            constexpr range(range<X, Y> rng)
              : compressed_pair<I, S>{detail::move(rng.first), detail::move(rng.second)}
            {}
            template<typename X, typename Y,
                CONCEPT_REQUIRES_(Constructible<I, X &&>() && Constructible<S, Y &&>())>
            constexpr range(std::pair<X, Y> rng)
              : compressed_pair<I, S>{detail::move(rng.first), detail::move(rng.second)}
            {}
            iterator begin() const
            {
                return first;
            }
            sentinel end() const
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
                CONCEPT_REQUIRES_(Constructible<X, I const &>() && Constructible<Y, S const &>())>
            constexpr operator std::pair<X, Y>() const
            {
                return std::pair<X, Y>{first, second};
            }
            iterator_reference_t<I> front() const
            {
                RANGES_ASSERT(!empty());
                return *first;
            }
            CONCEPT_REQUIRES(BidirectionalIterator<S>())
            iterator_reference_t<I> back() const
            {
                RANGES_ASSERT(!empty());
                return *prev(second);
            }
            // The requirements on the return type of I::operator[] are different
            // than for I::operator*, so respect that here.
            template<typename F = I, CONCEPT_REQUIRES_(RandomAccessIterator<I>())>
            auto operator[](iterator_difference_t<I> n) const ->
                decltype(std::declval<F>()[n])
            {
                return first[n];
            }
            CONCEPT_REQUIRES(!std::is_const<I>())
            void pop_front()
            {
                ++first;
            }
            CONCEPT_REQUIRES(!std::is_const<S>() && BidirectionalIterator<S>())
            void pop_back()
            {
                --second;
            }
        };

        // Like range, but with a known size. As with range and std::pair,
        // first and second are public members (for compatibility with old code using
        // pair to store iterator ranges), but for sized_range, the members are
        // const to prevent inadvertant violations of the class invariant.
        //
        // Class invariant:
        //   distance(first, second) == third
        //
        template<typename I, typename S /* = I */>
        struct sized_range
          : range<I const, S const>
        {
        private:
            using base = range<I const, S const>;
            I && move_first()
            {
                return detail::unsafe_move(this->first);
            }
            S && move_second()
            {
                return detail::unsafe_move(this->second);
            }
        public:
            using base::first;
            using base::second;
            iterator_size_t<I> const third;

            constexpr sized_range()
              : base{}, third{}
            {}
            constexpr sized_range(I begin, S end, iterator_size_t<I> size)
              : base{detail::move(begin), detail::move(end)}, third(size)
            {}
            constexpr sized_range(sized_range<I, S> const &rng)
              : base{rng.first, rng.second}, third(rng.third)
            {}
            constexpr sized_range(sized_range<I, S> &&rng)
              : base{rng.move_first(), rng.move_second()}, third(rng.third)
            {}
            template<typename X, typename Y,
                CONCEPT_REQUIRES_(Constructible<I, X &&>() && Constructible<S, Y &&>())>
            constexpr sized_range(X begin, Y end, iterator_size_t<I> size)
              : base{detail::move(begin), detail::move(end)}, third(size)
            {}
            template<typename X, typename Y,
                CONCEPT_REQUIRES_(Constructible<I, X &&>() && Constructible<S, Y &&>())>
            constexpr sized_range(std::pair<X, Y> rng, iterator_size_t<I> size)
              : base{detail::move(rng)}, third(size)
            {}
            template<typename X, typename Y,
                CONCEPT_REQUIRES_(Constructible<I, X &&>() && Constructible<S, Y &&>())>
            constexpr sized_range(range<X, Y> rng, iterator_size_t<I> size)
              : base{detail::move(rng.first), detail::move(rng.second)}, third(size)
            {}
            template<typename X, typename Y,
                CONCEPT_REQUIRES_(Constructible<I, X &&>() && Constructible<S, Y &&>())>
            constexpr sized_range(sized_range<X, Y> rng)
              : base{detail::move(rng.first), detail::move(rng.second)}, third(rng.third)
            {}
            sized_range &operator=(sized_range<I, S> const &rng)
            {
                const_cast<I &>(first) = rng.first;
                const_cast<S &>(second) = rng.second;
                const_cast<iterator_size_t<I> &>(third) = rng.third;
                return *this;
            }
            sized_range &operator=(sized_range<I, S> &&rng)
            {
                const_cast<I &>(first) = rng.move_first();
                const_cast<S &>(second) = rng.move_second();
                const_cast<iterator_size_t<I> &>(third) = rng.third;
                return *this;
            }
            template<typename X, typename Y,
                CONCEPT_REQUIRES_(Assignable<I &, X &&>() && Assignable<S &, Y &&>())>
            sized_range &operator=(sized_range<X, Y> rng)
            {
                const_cast<I &>(first) = rng.move_first();
                const_cast<S &>(second) = rng.move_second();
                const_cast<iterator_size_t<I> &>(third) = rng.third;
                return *this;
            }
            iterator_size_t<I> size() const
            {
                RANGES_ASSERT(!ForwardIterator<I>() ||
                    static_cast<iterator_size_t<I>>(iterator_range_distance(first, second)) == third);
                return third;
            }
            template<typename X, typename Y,
                CONCEPT_REQUIRES_(Constructible<X, I const &>() && Constructible<Y, S const &>())>
            constexpr operator std::pair<X, Y>() const
            {
                return std::pair<X, Y>{first, second};
            }
            template<typename X, typename Y,
                CONCEPT_REQUIRES_(Constructible<X, I const &>() && Constructible<Y, S const &>())>
            constexpr operator range<X, Y>() const
            {
                return range<X, Y>{first, second};
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
