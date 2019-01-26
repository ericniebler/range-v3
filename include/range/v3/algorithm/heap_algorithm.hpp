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
//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef RANGES_V3_ALGORITHM_HEAP_ALGORITHM_HPP
#define RANGES_V3_ALGORITHM_HEAP_ALGORITHM_HPP

#include <functional>
#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/iterator/operations.hpp>
#include <range/v3/range/access.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/range/dangling.hpp>
#include <range/v3/range/traits.hpp>
#include <range/v3/functional/comparisons.hpp>
#include <range/v3/functional/identity.hpp>
#include <range/v3/functional/invoke.hpp>
#include <range/v3/iterator/concepts.hpp>
#include <range/v3/iterator/traits.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    /// \cond
    namespace detail
    {
        struct is_heap_until_n_fn
        {
            template<typename I, typename C = less, typename P = identity>
            auto operator()(I const begin_, iter_difference_t<I> const n_, C pred = C{}, P proj = P{}) const ->
                CPP_ret(I)(
                    requires RandomAccessIterator<I> &&
                        IndirectStrictWeakOrder<C, projected<I, P>>)
            {
                RANGES_EXPECT(0 <= n_);
                iter_difference_t<I> p = 0, c = 1;
                I pp = begin_;
                while(c < n_)
                {
                    I cp = begin_ + c;
                    if(invoke(pred, invoke(proj, *pp), invoke(proj, *cp)))
                        return cp;
                    ++c;
                    ++cp;
                    if(c == n_ || invoke(pred, invoke(proj, *pp), invoke(proj, *cp)))
                        return cp;
                    ++p;
                    ++pp;
                    c = 2 * p + 1;
                }
                return begin_ + n_;
            }
        };

        RANGES_INLINE_VARIABLE(is_heap_until_n_fn, is_heap_until_n)

        struct is_heap_n_fn
        {
            template<typename I, typename C = less, typename P = identity>
            auto operator()(I begin, iter_difference_t<I> n, C pred = C{}, P proj = P{}) const ->
                CPP_ret(bool)(
                    requires RandomAccessIterator<I> &&
                        IndirectStrictWeakOrder<C, projected<I, P>>)
            {
                return is_heap_until_n(begin, n, std::move(pred), std::move(proj)) == begin + n;
            }
        };

        RANGES_INLINE_VARIABLE(is_heap_n_fn, is_heap_n)
    }
    /// \endcond

    /// \addtogroup group-algorithms
    /// @{
    struct is_heap_until_fn
    {
        template<typename I, typename S, typename C = less, typename P = identity>
        auto operator()(I begin, S end, C pred = C{}, P proj = P{}) const ->
            CPP_ret(I)(
                requires RandomAccessIterator<I> && Sentinel<S, I> &&
                    IndirectStrictWeakOrder<C, projected<I, P>>)
        {
            return detail::is_heap_until_n(std::move(begin), distance(begin, end), std::move(pred),
                std::move(proj));
        }

        template<typename Rng, typename C = less, typename P = identity>
        auto operator()(Rng &&rng, C pred = C{}, P proj = P{}) const ->
            CPP_ret(safe_iterator_t<Rng>)(
                requires RandomAccessRange<Rng> &&
                    IndirectStrictWeakOrder<C, projected<iterator_t<Rng>, P>>)
        {
            return detail::is_heap_until_n(begin(rng), distance(rng), std::move(pred),
                std::move(proj));
        }
    };

    /// \sa `is_heap_until_fn`
    /// \ingroup group-algorithms
    RANGES_INLINE_VARIABLE(is_heap_until_fn, is_heap_until)

    struct is_heap_fn
    {
        template<typename I, typename S, typename C = less, typename P = identity>
        auto operator()(I begin, S end, C pred = C{}, P proj = P{}) const ->
            CPP_ret(bool)(
                requires RandomAccessIterator<I> && Sentinel<S, I> &&
                    IndirectStrictWeakOrder<C, projected<I, P>>)
        {
            return detail::is_heap_n(std::move(begin), distance(begin, end), std::move(pred),
                std::move(proj));
        }

        template<typename Rng, typename C = less, typename P = identity>
        auto operator()(Rng &&rng, C pred = C{}, P proj = P{}) const ->
            CPP_ret(bool)(
                requires RandomAccessRange<Rng> &&
                    IndirectStrictWeakOrder<C, projected<iterator_t<Rng>, P>>)
        {
            return detail::is_heap_n(begin(rng), distance(rng), std::move(pred), std::move(proj));
        }
    };

    /// \sa `is_heap_fn`
    /// \ingroup group-algorithms
    RANGES_INLINE_VARIABLE(is_heap_fn, is_heap)
    /// @}

    /// \cond
    namespace detail
    {
        struct sift_up_n_fn
        {
            template<typename I, typename C = less, typename P = identity>
            void operator()(I begin, iter_difference_t<I> len, C pred = C{}, P proj = P{}) const
            {
                if(len > 1)
                {
                    I end = begin + len;
                    len = (len - 2) / 2;
                    I i = begin + len;
                    if(invoke(pred, invoke(proj, *i), invoke(proj, *--end)))
                    {
                        iter_value_t<I> v = iter_move(end);
                        do
                        {
                            *end = iter_move(i);
                            end = i;
                            if(len == 0)
                                break;
                            len = (len - 1) / 2;
                            i = begin + len;
                        } while(invoke(pred, invoke(proj, *i), invoke(proj, v)));
                        *end = std::move(v);
                    }
                }
            }
        };

        RANGES_INLINE_VARIABLE(sift_up_n_fn, sift_up_n)

        struct sift_down_n_fn
        {
            template<typename I, typename C = less, typename P = identity>
            void operator()(I begin, iter_difference_t<I> len, I start, C pred = C {}, P proj = P{}) const
            {
                // left-child of start is at 2 * start + 1
                // right-child of start is at 2 * start + 2
                auto child = start - begin;

                if(len < 2 || (len - 2) / 2 < child)
                    return;

                child = 2 * child + 1;
                I child_i = begin + child;

                if((child + 1) < len && invoke(pred, invoke(proj, *child_i), invoke(proj, *(child_i + 1))))
                {
                    // right-child exists and is greater than left-child
                    ++child_i;
                    ++child;
                }

                // check if we are in heap-order
                if(invoke(pred, invoke(proj, *child_i), invoke(proj, *start)))
                    // we are, start is larger than it's largest child
                    return;

                iter_value_t<I> top = iter_move(start);
                do
                {
                    // we are not in heap-order, swap the parent with it's largest child
                    *start = iter_move(child_i);
                    start = child_i;

                    if((len - 2) / 2 < child)
                        break;

                    // recompute the child based off of the updated parent
                    child = 2 * child + 1;
                    child_i = begin + child;

                    if((child + 1) < len && invoke(pred, invoke(proj, *child_i), invoke(proj, *(child_i + 1))))
                    {
                        // right-child exists and is greater than left-child
                        ++child_i;
                        ++child;
                    }

                    // check if we are in heap-order
                } while(!invoke(pred, invoke(proj, *child_i), invoke(proj, top)));
                *start = std::move(top);
            }
        };

        RANGES_INLINE_VARIABLE(sift_down_n_fn, sift_down_n)
    }
    /// \endcond

    /// \addtogroup group-algorithms
    /// @{
    struct push_heap_fn
    {
        template<typename I, typename S, typename C = less, typename P = identity>
        auto operator()(I begin, S end, C pred = C{}, P proj = P{}) const ->
            CPP_ret(I)(
                requires RandomAccessIterator<I> && Sentinel<S, I> && Sortable<I, C, P>)
        {
            auto n = distance(begin, end);
            detail::sift_up_n(begin, n, std::move(pred), std::move(proj));
            return begin + n;
        }

        template<typename Rng, typename C = less, typename P = identity>
        auto operator()(Rng &&rng, C pred = C{}, P proj = P{}) const ->
            CPP_ret(safe_iterator_t<Rng>)(
                requires RandomAccessRange<Rng> && Sortable<iterator_t<Rng>, C, P>)
        {
            iterator_t<Rng> begin = ranges::begin(rng);
            auto n = distance(rng);
            detail::sift_up_n(begin, n, std::move(pred), std::move(proj));
            return begin + n;
        }
    };

    /// \sa `push_heap_fn`
    /// \ingroup group-algorithms
    RANGES_INLINE_VARIABLE(push_heap_fn, push_heap)
    /// @}

    /// \cond
    namespace detail
    {
        struct pop_heap_n_fn
        {
            template<typename I, typename C = less, typename P = identity>
            auto operator()(I begin, iter_difference_t<I> len, C pred = C{},
                P proj = P{}) const ->
                CPP_ret(void)(
                    requires RandomAccessIterator<I> && Sortable<I, C, P>)
            {
                if(len > 1)
                {
                    ranges::iter_swap(begin, begin + (len-1));
                    detail::sift_down_n(begin, len-1, begin, std::move(pred), std::move(proj));
                }
            }
        };

        RANGES_INLINE_VARIABLE(pop_heap_n_fn, pop_heap_n)
    }
    /// \endcond

    /// \addtogroup group-algorithms
    /// @{
    struct pop_heap_fn
    {
        template<typename I, typename S, typename C = less, typename P = identity>
        auto operator()(I begin, S end, C pred = C{}, P proj = P{}) const ->
            CPP_ret(I)(
                requires RandomAccessIterator<I> && Sentinel<S, I> && Sortable<I, C, P>)
        {
            auto n = distance(begin, end);
            detail::pop_heap_n(begin, n, std::move(pred), std::move(proj));
            return begin + n;
        }

        template<typename Rng, typename C = less, typename P = identity>
        auto operator()(Rng &&rng, C pred = C{}, P proj = P{}) const ->
            CPP_ret(safe_iterator_t<Rng>)(
                requires RandomAccessRange<Rng> && Sortable<iterator_t<Rng>, C, P>)
        {
            iterator_t<Rng> begin = ranges::begin(rng);
            auto n = distance(rng);
            detail::pop_heap_n(begin, n, std::move(pred), std::move(proj));
            return begin + n;
        }
    };

    /// \sa `pop_heap_fn`
    /// \ingroup group-algorithms
    RANGES_INLINE_VARIABLE(pop_heap_fn, pop_heap)

    struct make_heap_fn
    {
        template<typename I, typename S, typename C = less, typename P = identity>
        auto operator()(I begin, S end, C pred = C{}, P proj = P{}) const ->
            CPP_ret(I)(
                requires RandomAccessIterator<I> && Sentinel<S, I> && Sortable<I, C, P>)
        {
            iter_difference_t<I> const n = distance(begin, end);
            if(n > 1)
                // start from the first parent, there is no need to consider children
                for(auto start = (n - 2) / 2; start >= 0; --start)
                    detail::sift_down_n(begin, n, begin + start, std::ref(pred), std::ref(proj));
            return begin + n;
        }

        template<typename Rng, typename C = less, typename P = identity>
        auto operator()(Rng &&rng, C pred = C{}, P proj = P{}) const ->
            CPP_ret(safe_iterator_t<Rng>)(
                requires RandomAccessRange<Rng> && Sortable<iterator_t<Rng>, C, P>)
        {
            iterator_t<Rng> begin = ranges::begin(rng);
            auto const n = distance(rng);
            if(n > 1)
                // start from the first parent, there is no need to consider children
                for(auto start = (n - 2) / 2; start >= 0; --start)
                    detail::sift_down_n(begin, n, begin + start, std::ref(pred), std::ref(proj));
            return begin + n;
        }
    };

    /// \sa `make_heap_fn`
    /// \ingroup group-algorithms
    RANGES_INLINE_VARIABLE(make_heap_fn, make_heap)

    struct sort_heap_fn
    {
        template<typename I, typename S, typename C = less, typename P = identity>
        auto operator()(I begin, S end, C pred = C{}, P proj = P{}) const ->
            CPP_ret(I)(
                requires RandomAccessIterator<I> && Sentinel<S, I> && Sortable<I, C, P>)
        {
            iter_difference_t<I> const n = distance(begin, end);
            for(auto i = n; i > 1; --i)
                detail::pop_heap_n(begin, i, std::ref(pred), std::ref(proj));
            return begin + n;
        }

        template<typename Rng, typename C = less, typename P = identity>
        auto operator()(Rng &&rng, C pred = C{}, P proj = P{}) const ->
            CPP_ret(safe_iterator_t<Rng>)(
                requires RandomAccessRange<Rng &> && Sortable<iterator_t<Rng>, C, P>)
        {
            iterator_t<Rng> begin = ranges::begin(rng);
            auto const n = distance(rng);
            for(auto i = n; i > 1; --i)
                detail::pop_heap_n(begin, i, std::ref(pred), std::ref(proj));
            return begin + n;
        }
    };

    /// \sa `sort_heap_fn`
    /// \ingroup group-algorithms
    RANGES_INLINE_VARIABLE(sort_heap_fn, sort_heap)
    /// @}
} // namespace ranges

#endif // include guard
