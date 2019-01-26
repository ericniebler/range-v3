/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2014-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
#ifndef RANGES_V3_ITERATOR_OPERATIONS_HPP
#define RANGES_V3_ITERATOR_OPERATIONS_HPP

#include <utility>
#include <type_traits>
#include <range/v3/range_fwd.hpp>
#include <range/v3/iterator/associated_types.hpp>
#include <range/v3/iterator/concepts.hpp>
#include <range/v3/range/concepts.hpp>

namespace ranges
{
    CPP_template(typename I)(
        requires Iterator<I>)
    struct counted_iterator;

    struct advance_fn
    {
#if RANGES_CXX_IF_CONSTEXPR >= RANGES_CXX_IF_CONSTEXPR_17
        template<typename I>
        constexpr /*c++14*/ auto operator()(I &i, iter_difference_t<I> n) const ->
            CPP_ret(void)(
                requires Iterator<I>)
        // [[expects: n >= 0 || BidirectionalIterator<I>]]
        {
            if constexpr(RandomAccessIterator<I>)
            {
                i += n;
            }
            else
            {
                if constexpr(BidirectionalIterator<I>)
                    for(; 0 > n; ++n)
                        --i;
                RANGES_EXPECT(0 <= n);
                for(; 0 < n; --n)
                    ++i;
            }
        }

        template<typename I, typename S>
        constexpr /*c++14*/ auto operator()(I &i, S bound) const ->
            CPP_ret(void)(
                requires Sentinel<S, I>)
        // [[expects axiom: reachable(i, bound)]]
        {
            if constexpr(Assignable<I&, S>)
            {
                i = std::move(bound);
            }
            else if constexpr(SizedSentinel<S, I>)
            {
                iter_difference_t<I> d = bound - i;
                RANGES_EXPECT(0 <= d);
                (*this)(i, d);
            }
            else
                while(i != bound)
                    ++i;
        }

        template<typename I, typename S>
        constexpr /*c++14*/
        auto operator()(I &i, iter_difference_t<I> n, S bound) const ->
            CPP_ret(iter_difference_t<I>)(
                requires Sentinel<S, I>)
        // [[expects axiom: 0 == n ||
        //     (0 < n && reachable(i, bound)) ||
        //     (0 > n && Same<I, S> && BidirectionalIterator<I> && reachable(bound, i))]]
        {
            if constexpr(SizedSentinel<S, I>)
            {
                const auto d = bound - i;
                if constexpr(BidirectionalIterator<I> && Same<I, S>)
                {
                    RANGES_EXPECT(0 <= n ? 0 <= d : 0 >= d);
                    if (0 <= n ? d <= n : d >= n)
                    {
                        i = std::move(bound);
                        return n - d;
                    }
                }
                else
                {
                    RANGES_EXPECT(0 <= n && 0 <= d);
                    if (d <= n)
                    {
                        (*this)(i, std::move(bound));
                        return n - d;
                    }
                }
                (*this)(i, n);
                return 0;
            }
            else
            {
                if constexpr(BidirectionalIterator<I> && Same<I, S>)
                {
                    if (0 > n)
                    {
                        do
                        {
                            --i;
                            ++n;
                        }
                        while(0 != n && i != bound);
                        return n;
                    }
                }
                RANGES_EXPECT(0 <= n);
                while(0 != n && i != bound)
                {
                    ++i;
                    --n;
                }
                return n;
            }
        }
#else
    private:
        template<typename I>
        static constexpr /*c++14*/
        void n_(I &i, iter_difference_t<I> n,
            detail::input_iterator_tag);
        template<typename I>
        static constexpr /*c++14*/
        void n_(I &i, iter_difference_t<I> n,
            detail::bidirectional_iterator_tag);
        template<typename I>
        static constexpr /*c++14*/
        void n_(I &i, iter_difference_t<I> n,
            detail::random_access_iterator_tag);
        template<typename I, typename S>
        static constexpr /*c++14*/
        void to_impl_(I &i, S s, sentinel_tag);
        template<typename I, typename S>
        static constexpr /*c++14*/
        void to_impl_(I &i, S s, sized_sentinel_tag);
        template<typename I, typename S>
        static constexpr /*c++14*/
        void to_(I &i, S s, std::true_type);// assignable
        template<typename I, typename S>
        static constexpr /*c++14*/
        void to_(I &i, S s, std::false_type);// !assignable
        template<typename I, typename D, typename S>
        static constexpr /*c++14*/
        D bounded_(I &it, D n, S bound, sentinel_tag,
            detail::input_iterator_tag);
        template<typename I, typename D>
        static constexpr /*c++14*/
        D bounded_(I &it, D n, I bound, sentinel_tag,
            detail::bidirectional_iterator_tag);
        template<typename I, typename D, typename S, typename Concept>
        static constexpr /*c++14*/
        D bounded_(I &it, D n, S bound, sized_sentinel_tag,
            Concept);
    public:
        // Advance a certain number of steps:
        template<typename I>
        constexpr /*c++14*/
        auto operator()(I &i, iter_difference_t<I> n) const ->
            CPP_ret(void)(
                requires Iterator<I>)
        {
            advance_fn::n_(i, n, iterator_tag_of<I>{});
        }
        // Advance to a certain position:
        template<typename I, typename S>
        constexpr /*c++14*/
        auto operator()(I &i, S s) const ->
            CPP_ret(void)(
                requires Sentinel<S, I>)
        {
            advance_fn::to_(i, static_cast<S &&>(s), meta::bool_<Assignable<I&, S>>());
        }
        // Advance a certain number of times, with a bound:
        template<typename I, typename S>
        constexpr /*c++14*/
        auto operator()(I &it, iter_difference_t<I> n, S bound) const ->
            CPP_ret(iter_difference_t<I>)(
                requires Sentinel<S, I>)
        {
            return advance_fn::bounded_(it, n, static_cast<S &&>(bound),
                sentinel_tag_of<S, I>(), iterator_tag_of<I>());
        }
#endif

        template<typename I>
        constexpr /*c++14*/
        auto operator()(counted_iterator<I>& i, iter_difference_t<I> n) const ->
            CPP_ret(void)(
                requires Iterator<I>);
    };

    /// \addtogroup group-utility
    /// @{

    /// \ingroup group-utility
    /// \sa `advance_fn`
    /// Not to spec: advance is an ADL customization point
    RANGES_INLINE_VARIABLE(advance_fn, advance)

#if RANGES_CXX_IF_CONSTEXPR < RANGES_CXX_IF_CONSTEXPR_17
    template<typename I>
    inline constexpr /*c++14*/
    void advance_fn::n_(I &i, iter_difference_t<I> n,
        detail::input_iterator_tag)
    {
        RANGES_EXPECT(n >= 0);
        for(; n > 0; --n)
            ++i;
    }
    template<typename I>
    inline constexpr /*c++14*/
    void advance_fn::n_(I &i, iter_difference_t<I> n,
        detail::bidirectional_iterator_tag)
    {
        if(n > 0)
            for(; n > 0; --n)
                ++i;
        else
            for(; n < 0; ++n)
                --i;
    }
    template<typename I>
    inline constexpr /*c++14*/
    void advance_fn::n_(I &i, iter_difference_t<I> n,
        detail::random_access_iterator_tag)
    {
        i += n;
    }
    template<typename I, typename S>
    inline constexpr /*c++14*/
    void advance_fn::to_impl_(I &i, S s, sentinel_tag)
    {
        while(i != s)
            ++i;
    }
    template<typename I, typename S>
    inline constexpr /*c++14*/
    void advance_fn::to_impl_(I &i, S s, sized_sentinel_tag)
    {
        iter_difference_t<I> d = s - i;
        RANGES_EXPECT(0 <= d);
        advance(i, d);
    }
    // Advance to a certain position:
    template<typename I, typename S>
    inline constexpr /*c++14*/
    void advance_fn::to_(I &i, S s, std::true_type)
    {
        i = static_cast<S &&>(s);
    }
    template<typename I, typename S>
    constexpr /*c++14*/
    void advance_fn::to_(I &i, S s, std::false_type)
    {
        advance_fn::to_impl_(i, static_cast<S &&>(s), sentinel_tag_of<S, I>());
    }
    template<typename I, typename D, typename S>
    inline constexpr /*c++14*/
    D advance_fn::bounded_(I &it, D n, S bound, sentinel_tag,
        detail::input_iterator_tag)
    {
        RANGES_EXPECT(0 <= n);
        for(; 0 != n && it != bound; --n)
            ++it;
        return n;
    }
    template<typename I, typename D>
    inline constexpr /*c++14*/
    D advance_fn::bounded_(I &it, D n, I bound, sentinel_tag,
        detail::bidirectional_iterator_tag)
    {
        if(0 <= n)
            for(; 0 != n && it != bound; --n)
                ++it;
        else
            for(; 0 != n && it != bound; ++n)
                --it;
        return n;
    }
    template<typename I, typename D, typename S, typename Concept>
    inline constexpr /*c++14*/
    D advance_fn::bounded_(I &it, D n, S bound, sized_sentinel_tag,
        Concept)
    {
        RANGES_EXPECT(((bool)Same<I, S> || 0 <= n));
        D d = bound - it;
        RANGES_EXPECT(0 <= n ? 0 <= d : 0 >= d);
        if(0 <= n ? n >= d : n <= d)
        {
            advance(it, static_cast<S &&>(bound));
            return n - d;
        }
        advance(it, n);
        return 0;
    }
#endif

    struct next_fn
    {
        template<typename I>
        constexpr /*c++14*/
        auto operator()(I it) const ->
            CPP_ret(I)(requires Iterator<I>)
        {
            return ++it;
        }
        template<typename I>
        constexpr /*c++14*/
        auto operator()(I it, iter_difference_t<I> n) const ->
            CPP_ret(I)(requires Iterator<I>)
        {
            advance(it, n);
            return it;
        }
        template<typename I, typename S>
        constexpr /*c++14*/
        auto operator()(I it, S s) const ->
            CPP_ret(I)(requires Sentinel<S, I>)
        {
            advance(it, static_cast<S &&>(s));
            return it;
        }
        template<typename I, typename S>
        constexpr /*c++14*/
        auto operator()(I it, iter_difference_t<I> n, S bound) const ->
            CPP_ret(I)(requires Sentinel<S, I>)
        {
            advance(it, n, static_cast<S &&>(bound));
            return it;
        }
    };

    /// \ingroup group-utility
    /// \sa `next_fn`
    RANGES_INLINE_VARIABLE(next_fn, next)

    struct prev_fn
    {
        template<typename I>
        constexpr /*c++14*/
        auto operator()(I it) const ->
            CPP_ret(I)(requires BidirectionalIterator<I>)
        {
            return --it;
        }
        template<typename I>
        constexpr /*c++14*/
        auto operator()(I it, iter_difference_t<I> n) const ->
            CPP_ret(I)(requires BidirectionalIterator<I>)
        {
            advance(it, -n);
            return it;
        }
        template<typename I>
        constexpr /*c++14*/
        auto operator()(I it, iter_difference_t<I> n, I bound) const ->
            CPP_ret(I)(requires BidirectionalIterator<I>)
        {
            advance(it, -n, static_cast<I &&>(bound));
            return it;
        }
    };

    /// \ingroup group-utility
    /// \sa `prev_fn`
    RANGES_INLINE_VARIABLE(prev_fn, prev)

    struct iter_enumerate_fn
    {
    private:
        template<typename I, typename S, typename D>
        static constexpr /*c++14*/
        auto impl_i(I begin, S end, D d, sentinel_tag) ->
            CPP_ret(std::pair<D, I>)(
                requires not SizedSentinel<I, I>)
        {
            for(; begin != end; ++begin)
                ++d;
            return {d, begin};
        }
        template<typename I, typename S, typename D>
        static constexpr /*c++14*/
        auto impl_i(I begin, S end_, D d, sentinel_tag) ->
            CPP_ret(std::pair<D, I>)(
                requires SizedSentinel<I, I>)
        {
            I end = ranges::next(begin, end_);
            auto n = static_cast<D>(end - begin);
            RANGES_EXPECT(((bool)Same<I, S> || 0 <= n));
            return {n + d, end};
        }
        template<typename I, typename S, typename D>
        static constexpr /*c++14*/
        std::pair<D, I> impl_i(I begin, S end, D d, sized_sentinel_tag)
        {
            auto n = static_cast<D>(end - begin);
            RANGES_EXPECT(((bool)Same<I, S> || 0 <= n));
            return {n + d, ranges::next(begin, end)};
        }
    public:
        template<typename I, typename S, typename D = iter_difference_t<I>>
        constexpr /*c++14*/
        auto operator()(I begin, S end, D d = 0) const ->
            CPP_ret(std::pair<D, I>)(
                requires Sentinel<S, I> && Integral<D>)
        {
            return iter_enumerate_fn::impl_i(static_cast<I &&>(begin),
                static_cast<S &&>(end), d, sentinel_tag_of<S, I>());
        }
    };

    /// \ingroup group-utility
    /// \sa `iter_enumerate_fn`
    RANGES_INLINE_VARIABLE(iter_enumerate_fn, iter_enumerate)

    struct iter_distance_fn
    {
    private:
        template<typename I, typename S, typename D>
        static constexpr /*c++14*/
        D impl_i(I begin, S end, D d, sentinel_tag)
        {
            return iter_enumerate(static_cast<I &&>(begin), static_cast<S &&>(end), d).first;
        }
        template<typename I, typename S, typename D>
        static constexpr /*c++14*/
        D impl_i(I begin, S end, D d, sized_sentinel_tag)
        {
            auto n = static_cast<D>(end - begin);
            RANGES_EXPECT(((bool)Same<I, S> || 0 <= n));
            return n + d;
        }
    public:
        template<typename I, typename S, typename D = iter_difference_t<I>>
        constexpr /*c++14*/
        auto operator()(I begin, S end, D d = 0) const ->
            CPP_ret(D)(
                requires Iterator<I> && Sentinel<S, I> && Integral<D>)
        {
            return iter_distance_fn::impl_i(static_cast<I &&>(begin),
                static_cast<S &&>(end), d, sentinel_tag_of<S, I>());
        }
    };

    /// \ingroup group-utility
    /// \sa `iter_distance_fn`
    RANGES_INLINE_VARIABLE(iter_distance_fn, iter_distance)

    struct iter_distance_compare_fn
    {
    private:
        template<typename I, typename S>
        static constexpr /*c++14*/
        int impl_i(I begin, S end, iter_difference_t<I> n, sentinel_tag)
        {
            if(n < 0)
                return 1;
            for(; n > 0; --n, ++begin)
            {
                if(begin == end)
                    return -1;
            }
            return begin == end ? 0 : 1;
        }
        template<typename I, typename S>
        static constexpr /*c++14*/
        int impl_i(I begin, S end, iter_difference_t<I> n, sized_sentinel_tag)
        {
            iter_difference_t<I> dist = end - begin;
            if(n < dist)
                return  1;
            if(dist < n)
                return -1;
            return  0;
        }
    public:
        template<typename I, typename S>
        constexpr /*c++14*/
        auto operator()(I begin, S end, iter_difference_t<I> n) const ->
            CPP_ret(int)(
                requires InputIterator<I> && Sentinel<S, I>)
        {
            return iter_distance_compare_fn::impl_i(static_cast<I &&>(begin),
                static_cast<S &&>(end), n, sentinel_tag_of<S, I>());
        }
    };

    /// \ingroup group-utility
    /// \sa `iter_distance_compare_fn`
    RANGES_INLINE_VARIABLE(iter_distance_compare_fn, iter_distance_compare)

    // Like distance(b,e), but guaranteed to be O(1)
    struct iter_size_fn
    {
        template<typename I, typename S>
        constexpr /*c++14*/
        auto operator()(I const &begin, S end) const ->
            CPP_ret(meta::_t<std::make_unsigned<iter_difference_t<I>>>)(
                requires SizedSentinel<S, I>)
        {
            using size_type =
                meta::_t<std::make_unsigned<iter_difference_t<I>>>;
            iter_difference_t<I> n = end - begin;
            RANGES_EXPECT(0 <= n);
            return static_cast<size_type>(n);
        }
    };

    /// \ingroup group-utility
    /// \sa `iter_size_fn`
    RANGES_INLINE_VARIABLE(iter_size_fn, iter_size)

    /// \cond
    namespace adl_uncounted_recounted_detail
    {
        template<typename I>
        constexpr I uncounted(I i)
        {
            return i;
        }

        template<typename I>
        constexpr I recounted(I const &, I i, iter_difference_t<I>)
        {
            return i;
        }

        struct uncounted_fn
        {
            template<typename I>
            constexpr auto operator()(I i) const ->
                decltype(uncounted((I &&)i))
            {
                return uncounted((I &&)i);
            }
        };

        struct recounted_fn
        {
            template<typename I, typename J>
            constexpr
            auto operator()(I i, J j, iter_difference_t<J> n) const ->
                decltype(recounted((I &&)i, (J &&)j, n))
            {
                return recounted((I &&)i, (J &&)j, n);
            }
        };
    }
    /// \endcond

    /// \addtogroup group-utility
    /// @{
    RANGES_INLINE_VARIABLE(adl_uncounted_recounted_detail::uncounted_fn,
                           uncounted)
    RANGES_INLINE_VARIABLE(adl_uncounted_recounted_detail::recounted_fn,
                           recounted)
    /// @}

    /// \addtogroup group-core
    /// @{
    struct enumerate_fn : iter_enumerate_fn
    {
    private:
        template<typename Rng, typename D>
        static std::pair<D, iterator_t<Rng>>
        impl_r(Rng &rng, D d, range_tag, range_tag)
        {
            return iter_enumerate(begin(rng), end(rng), d);
        }
        template<typename Rng, typename D>
        static std::pair<D, iterator_t<Rng>>
        impl_r(Rng &rng, D d, common_range_tag, sized_range_tag)
        {
            return {static_cast<D>(size(rng)) + d, end(rng)};
        }
    public:
        using iter_enumerate_fn::operator();

        template<typename Rng, typename D = range_difference_t<Rng>>
        auto operator()(Rng &&rng, D d = 0) const ->
            CPP_ret(std::pair<D, iterator_t<Rng>>)(
                requires Integral<D> && Range<Rng>)
        {
            // Better not be trying to compute the distance of an infinite range:
            RANGES_EXPECT(!is_infinite<Rng>::value);
            auto result = enumerate_fn::impl_r(rng, d, common_range_tag_of<Rng>(),
                sized_range_tag_of<Rng>());
            RANGES_EXPECT(result.first >= d);
            return result;
        }
    };

    /// \ingroup group-core
    /// \sa `enumerate_fn`
    RANGES_INLINE_VARIABLE(enumerate_fn, enumerate)

    struct distance_fn : iter_distance_fn
    {
    private:
        template<typename Rng, typename D>
        static D impl_r(Rng &rng, D d, range_tag)
        {
            return enumerate(rng, d).first;
        }
        template<typename Rng, typename D>
        constexpr /*c++14*/
        static D impl_r(Rng &rng, D d, sized_range_tag)
        {
            return static_cast<D>(size(rng)) + d;
        }
    public:
        using iter_distance_fn::operator();

        template<typename Rng, typename D = range_difference_t<Rng>>
        constexpr /*c++14*/
        auto operator()(Rng &&rng, D d = 0) const ->
            CPP_ret(D)(
                requires Integral<D> && Range<Rng>)
        {
            // Better not be trying to compute the distance of an infinite range:
            RANGES_EXPECT(!is_infinite<Rng>::value);
            auto result = distance_fn::impl_r(rng, d, sized_range_tag_of<Rng>());
            RANGES_EXPECT(result >= d);
            return result;
        }
    };

    /// \ingroup group-core
    /// \sa `distance_fn`
    RANGES_INLINE_VARIABLE(distance_fn, distance)

    // The interface of distance_compare is taken from Util.listLengthCmp in the GHC API.
    struct distance_compare_fn : iter_distance_compare_fn
    {
    private:
        template<typename Rng>
        static int impl_r(Rng &rng, range_difference_t<Rng> n, range_tag)
        {
            // Infinite ranges are always compared to be larger than a finite number.
            return is_infinite<Rng>::value
                ? 1
                : iter_distance_compare(begin(rng), end(rng), n);
        }
        template<typename Rng>
        static int impl_r(Rng &rng, range_difference_t<Rng> n, sized_range_tag)
        {
            auto dist = distance(rng); // O(1) since rng is a SizedRange
            if(dist > n)
                return  1;
            else if(dist < n)
                return -1;
            else
                return  0;
        }
    public:
        using iter_distance_compare_fn::operator();

        template<typename Rng>
        auto operator()(Rng &&rng, range_difference_t<Rng> n) const ->
            CPP_ret(int)(
                requires Range<Rng>)
        {
            return distance_compare_fn::impl_r(rng, n, sized_range_tag_of<Rng>());
        }
    };

    /// \ingroup group-core
    /// \sa `distance_compare_fn`
    RANGES_INLINE_VARIABLE(distance_compare_fn, distance_compare)
    /// @}
} // namespace ranges

#endif // RANGES_V3_ITERATOR_OPERATIONS_HPP
