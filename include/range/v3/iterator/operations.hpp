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

namespace ranges
{
    /// \cond
    namespace adl_advance_detail
    {
#ifdef RANGES_WORKAROUND_MSVC_620035
        void advance();
#endif

        template<typename I, typename D>
        void advance(I&, D) = delete;

        struct advance_fn
        {
        private:
            template<typename I>
            constexpr /*c++14*/
            static void n_impl_(I &i, iter_difference_t<I> n,
                detail::input_iterator_tag);
            template<typename I>
            constexpr /*c++14*/
            static void n_impl_(I &i, iter_difference_t<I> n,
                detail::bidirectional_iterator_tag);
            template<typename I>
            constexpr /*c++14*/
            static void n_impl_(I &i, iter_difference_t<I> n,
                detail::random_access_iterator_tag);
            // Is there an advance that is find-able by ADL and is preferred
            // by partial ordering to the poison-pill overload?
            template<typename I>
            constexpr /*c++14*/
            static auto n_(I &i, iter_difference_t<I> n, int) ->
                decltype(static_cast<void>(advance(i, n)));
            // No advance overload found by ADL, use the default implementation:
            template<typename I>
            constexpr /*c++14*/
            static void n_(I &i, iter_difference_t<I> n, long);
            template<typename I, typename S>
            constexpr /*c++14*/
            static void to_(I &i, S s, sentinel_tag);
            template<typename I, typename S>
            constexpr /*c++14*/
            static void to_(I &i, S s, sized_sentinel_tag);
            template<typename I, typename D, typename S>
            constexpr /*c++14*/
            static D bounded_(I &it, D n, S bound, sentinel_tag,
                detail::input_iterator_tag);
            template<typename I, typename D>
            constexpr /*c++14*/
            static D bounded_(I &it, D n, I bound, sentinel_tag,
                detail::bidirectional_iterator_tag);
            template<typename I, typename D, typename S, typename Concept>
            constexpr /*c++14*/
            static D bounded_(I &it, D n, S bound, sized_sentinel_tag,
                Concept);
        public:
            // Advance a certain number of steps:
            template<typename I>
            constexpr /*c++14*/
            auto operator()(I &i, iter_difference_t<I> n) const ->
                CPP_ret(void)(requires Iterator<I>)
            {
                advance_fn::n_(i, n, 0);
            }
            // Advance to a certain position:
            template<typename I, typename S>
            constexpr /*c++14*/
            auto operator()(I &i, S s) const ->
                CPP_ret(void)(
                    requires Sentinel<S, I> && Assignable<I&, S>)
            {
                i = std::move(s);
            }
            template<typename I, typename S>
            constexpr /*c++14*/
            auto operator()(I &i, S s) const ->
                CPP_ret(void)(
                    requires Sentinel<S, I> && !Assignable<I&, S>)
            {
                advance_fn::to_(i, std::move(s), sentinel_tag_of<S, I>());
            }
            // Advance a certain number of times, with a bound:
            template<typename I, typename S>
            constexpr /*c++14*/
            auto operator()(I &it, iter_difference_t<I> n, S bound) const ->
                CPP_ret(iter_difference_t<I>)(
                    requires Sentinel<S, I>)
            {
                return advance_fn::bounded_(it, n, std::move(bound),
                    sentinel_tag_of<S, I>(), iterator_tag_of<I>());
            }
        };
    }
    /// \endcond

    /// \addtogroup group-utility
    /// @{

    /// \ingroup group-utility
    /// \sa `advance_fn`
    /// Not to spec: advance is an ADL customization point
    RANGES_DEFINE_CPO(adl_advance_detail::advance_fn, advance)

    /// \cond
    namespace adl_advance_detail
    {
        template<typename I>
        constexpr /*c++14*/
        void advance_fn::n_impl_(I &i, iter_difference_t<I> n,
            detail::input_iterator_tag)
        {
            RANGES_EXPECT(n >= 0);
            for(; n > 0; --n)
                ++i;
        }
        template<typename I>
        constexpr /*c++14*/
        void advance_fn::n_impl_(I &i, iter_difference_t<I> n,
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
        constexpr /*c++14*/
        void advance_fn::n_impl_(I &i, iter_difference_t<I> n,
            detail::random_access_iterator_tag)
        {
            i += n;
        }
        // Is there an advance that is find-able by ADL and is preferred
        // by partial ordering to the poison-pill overload?
        template<typename I>
        constexpr /*c++14*/
        auto advance_fn::n_(I &i, iter_difference_t<I> n, int) ->
            decltype(static_cast<void>(advance(i, n)))
        {
            advance(i, n);
        }
        // No advance overload found by ADL, use the default implementation:
        template<typename I>
        constexpr /*c++14*/
        void advance_fn::n_(I &i, iter_difference_t<I> n, long)
        {
            advance_fn::n_impl_(i, n, iterator_tag_of<I>{});
        }
        template<typename I, typename S>
        constexpr /*c++14*/
        void advance_fn::to_(I &i, S s, sentinel_tag)
        {
            while(i != s)
                ++i;
        }
        template<typename I, typename S>
        constexpr /*c++14*/
        void advance_fn::to_(I &i, S s, sized_sentinel_tag)
        {
            iter_difference_t<I> d = s - i;
            RANGES_EXPECT(0 <= d);
            ranges::advance(i, d);
        }
        template<typename I, typename D, typename S>
        constexpr /*c++14*/
        D advance_fn::bounded_(I &it, D n, S bound, sentinel_tag,
            detail::input_iterator_tag)
        {
            RANGES_EXPECT(0 <= n);
            for(; 0 != n && it != bound; --n)
                ++it;
            return n;
        }
        template<typename I, typename D>
        constexpr /*c++14*/
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
        constexpr /*c++14*/
        D advance_fn::bounded_(I &it, D n, S bound, sized_sentinel_tag,
            Concept)
        {
            RANGES_EXPECT(((bool)Same<I, S> || 0 <= n));
            D d = bound - it;
            RANGES_EXPECT(0 <= n ? 0 <= d : 0 >= d);
            if(0 <= n ? n >= d : n <= d)
            {
                ranges::advance(it, std::move(bound));
                return n - d;
            }
            ranges::advance(it, n);
            return 0;
        }
    }
    /// \endcond

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
            advance(it, std::move(s));
            return it;
        }
        template<typename I, typename S>
        constexpr /*c++14*/
        auto operator()(I it, iter_difference_t<I> n, S bound) const ->
            CPP_ret(I)(requires Sentinel<S, I>)
        {
            advance(it, n, std::move(bound));
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
            advance(it, -n, std::move(bound));
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
        constexpr /*c++14*/
        auto impl_i(I begin, S end, D d, sentinel_tag) const ->
            CPP_ret(std::pair<D, I>)(
                requires not SizedSentinel<I, I>)
        {
            for(; begin != end; ++begin)
                ++d;
            return {d, begin};
        }
        template<typename I, typename S, typename D>
        constexpr /*c++14*/
        auto impl_i(I begin, S end_, D d, sentinel_tag) const ->
            CPP_ret(std::pair<D, I>)(
                requires SizedSentinel<I, I>)
        {
            I end = ranges::next(begin, end_);
            auto n = static_cast<D>(end - begin);
            RANGES_EXPECT(((bool)Same<I, S> || 0 <= n));
            return {n + d, end};
        }
        template<typename I, typename S, typename D>
        constexpr /*c++14*/
        std::pair<D, I> impl_i(I begin, S end, D d, sized_sentinel_tag) const
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
                requires Iterator<I> && Sentinel<S, I> && Integral<D>)
        {
            return this->impl_i(std::move(begin), std::move(end), d,
                sentinel_tag_of<S, I>());
        }
    };

    /// \ingroup group-utility
    /// \sa `iter_enumerate_fn`
    RANGES_INLINE_VARIABLE(iter_enumerate_fn, iter_enumerate)

    struct iter_distance_fn
    {
    private:
        template<typename I, typename S, typename D>
        constexpr /*c++14*/
        D impl_i(I begin, S end, D d, sentinel_tag) const
        {
            return iter_enumerate(std::move(begin), std::move(end), d).first;
        }
        template<typename I, typename S, typename D>
        constexpr /*c++14*/
        D impl_i(I begin, S end, D d, sized_sentinel_tag) const
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
            return this->impl_i(std::move(begin), std::move(end), d,
                sentinel_tag_of<S, I>());
        }
    };

    /// \ingroup group-utility
    /// \sa `iter_distance_fn`
    RANGES_INLINE_VARIABLE(iter_distance_fn, iter_distance)

    struct iter_distance_compare_fn
    {
    private:
        template<typename I, typename S>
        constexpr /*c++14*/
        int impl_i(I begin, S end, iter_difference_t<I> n, sentinel_tag) const
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
        constexpr /*c++14*/
        int impl_i(I begin, S end, iter_difference_t<I> n, sized_sentinel_tag) const
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
            return this->impl_i(std::move(begin), std::move(end), n,
                sentinel_tag_of<S, I>());
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
            CPP_ret(typename std::make_unsigned<iter_difference_t<I>>::type)(
                requires SizedSentinel<S, I>)
        {
            using size_type =
                typename std::make_unsigned<iter_difference_t<I>>::type;
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
            constexpr
            auto operator()(I i) const ->
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
} // namespace ranges

#endif // RANGES_V3_ITERATOR_OPERATIONS_HPP
