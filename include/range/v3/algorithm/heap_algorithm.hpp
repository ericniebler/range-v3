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
#include <range/v3/distance.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/iterator.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \ingroup group-concepts
        template<typename I, typename C = ordered_less, typename P = ident>
        using IsHeapable = meta::fast_and<
            RandomAccessIterator<I>,
            IndirectCallableRelation<C, Project<I, P>>>;

        /// \cond
        namespace detail
        {
            struct is_heap_until_n_fn
            {
                template<typename I, typename C = ordered_less, typename P = ident,
                    CONCEPT_REQUIRES_(IsHeapable<I, C, P>())>
                RANGES_RELAXED_CONSTEXPR
                I operator()(I const begin_, iterator_difference_t<I> const n_, C pred_ = C{}, P proj_ = P{}) const
                {
                    RANGES_ASSERT(0 <= n_);
                    auto &&pred = as_function(pred_);
                    auto &&proj = as_function(proj_);
                    iterator_difference_t<I> p = 0, c = 1;
                    I pp = begin_;
                    while(c < n_)
                    {
                        I cp = begin_ + c;
                        if(pred(proj(*pp), proj(*cp)))
                            return cp;
                        ++c;
                        ++cp;
                        if(c == n_ || pred(proj(*pp), proj(*cp)))
                            return cp;
                        ++p;
                        ++pp;
                        c = 2 * p + 1;
                    }
                    return begin_ + n_;
                }
            };

            namespace
            {
                constexpr auto&& is_heap_until_n = static_const<is_heap_until_n_fn>::value;
            }

            struct is_heap_n_fn
            {
                template<typename I, typename C = ordered_less, typename P = ident,
                    CONCEPT_REQUIRES_(IsHeapable<I, C, P>())>
                RANGES_RELAXED_CONSTEXPR
                bool operator()(I begin, iterator_difference_t<I> n, C pred = C{}, P proj = P{}) const
                {
                    return is_heap_until_n(begin, n, std::move(pred), std::move(proj)) == begin + n;
                }
            };

            namespace
            {
                constexpr auto&& is_heap_n = static_const<is_heap_n_fn>::value;
            }
        }
        /// \endcond

        /// \addtogroup group-algorithms
        /// @{
        struct is_heap_until_fn
        {
            template<typename I, typename S, typename C = ordered_less, typename P = ident,
                CONCEPT_REQUIRES_(IsHeapable<I, C, P>() && IteratorRange<I, S>())>
            RANGES_RELAXED_CONSTEXPR
            I operator()(I begin, S end, C pred = C{}, P proj = P{}) const
            {
                return detail::is_heap_until_n(std::move(begin), distance(begin, end), std::move(pred),
                    std::move(proj));
            }

            template<typename Rng, typename C = ordered_less, typename P = ident,
                typename I = range_iterator_t<Rng>,
                CONCEPT_REQUIRES_(IsHeapable<I, C, P>() && Iterable<Rng>())>
            RANGES_RELAXED_CONSTEXPR
            range_safe_iterator_t<Rng> operator()(Rng &&rng, C pred = C{}, P proj = P{}) const
            {
                return detail::is_heap_until_n(begin(rng), distance(rng), std::move(pred),
                    std::move(proj));
            }
        };

        /// \sa `is_heap_until_fn`
        /// \ingroup group-algorithms
        namespace
        {
            constexpr auto&& is_heap_until = static_const<with_braced_init_args<is_heap_until_fn>>::value;
        }

        struct is_heap_fn
        {
            template<typename I, typename S, typename C = ordered_less, typename P = ident,
                CONCEPT_REQUIRES_(IsHeapable<I, C, P>() && IteratorRange<I, S>())>
            RANGES_RELAXED_CONSTEXPR
            bool operator()(I begin, S end, C pred = C{}, P proj = P{}) const
            {
                return detail::is_heap_n(std::move(begin), distance(begin, end), std::move(pred),
                    std::move(proj));
            }

            template<typename Rng, typename C = ordered_less, typename P = ident,
                typename I = range_iterator_t<Rng>,
                CONCEPT_REQUIRES_(IsHeapable<I, C, P>() && Iterable<Rng>())>
            RANGES_RELAXED_CONSTEXPR
            bool operator()(Rng &&rng, C pred = C{}, P proj = P{}) const
            {
                return detail::is_heap_n(begin(rng), distance(rng), std::move(pred), std::move(proj));
            }
        };

        /// \sa `is_heap_fn`
        /// \ingroup group-algorithms
        namespace
        {
            constexpr auto&& is_heap = static_const<with_braced_init_args<is_heap_fn>>::value;
        }
        /// @}

        /// \cond
        namespace detail
        {
            struct sift_up_n_fn
            {
                template<typename I, typename C = ordered_less, typename P = ident>
                RANGES_RELAXED_CONSTEXPR
                void operator()(I begin, iterator_difference_t<I> len, C pred_ = C{}, P proj_ = P{}) const
                {
                    if(len > 1)
                    {
                        auto &&pred = as_function(pred_);
                        auto &&proj = as_function(proj_);
                        I end = begin + len;
                        len = (len - 2) / 2;
                        I i = begin + len;
                        if(pred(proj(*i), proj(*--end)))
                        {
                            iterator_value_t<I> v = iter_move(end);
                            do
                            {
                                *end = iter_move(i);
                                end = i;
                                if(len == 0)
                                    break;
                                len = (len - 1) / 2;
                                i = begin + len;
                            } while(pred(proj(*i), proj(v)));
                            *end = std::move(v);
                        }
                    }
                }
            };

            namespace
            {
                constexpr auto&& sift_up_n = static_const<sift_up_n_fn>::value;
            }

            struct sift_down_n_fn
            {
                template<typename I, typename C = ordered_less, typename P = ident>
                RANGES_RELAXED_CONSTEXPR
                void operator()(I begin, iterator_difference_t<I> len, I start, C pred_ = C {}, P proj_ = P{}) const
                {
                    // left-child of start is at 2 * start + 1
                    // right-child of start is at 2 * start + 2
                    auto child = start - begin;

                    if(len < 2 || (len - 2) / 2 < child)
                        return;

                    child = 2 * child + 1;
                    I child_i = begin + child;

                    auto &&pred = as_function(pred_);
                    auto &&proj = as_function(proj_);

                    if((child + 1) < len && pred(proj(*child_i), proj(*(child_i + 1))))
                    {
                        // right-child exists and is greater than left-child
                        ++child_i;
                        ++child;
                    }

                    // check if we are in heap-order
                    if(pred(proj(*child_i), proj(*start)))
                        // we are, start is larger than it's largest child
                        return;

                    iterator_value_t<I> top = iter_move(start);
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

                        if((child + 1) < len && pred(proj(*child_i), proj(*(child_i + 1))))
                        {
                            // right-child exists and is greater than left-child
                            ++child_i;
                            ++child;
                        }

                        // check if we are in heap-order
                    } while (!pred(proj(*child_i), proj(top)));
                    *start = std::move(top);
                }
            };

            namespace
            {
                constexpr auto&& sift_down_n = static_const<sift_down_n_fn>::value;
            }
        }
        /// \endcond

        /// \addtogroup group-algorithms
        /// @{
        struct push_heap_fn
        {
            template<typename I, typename S, typename C = ordered_less, typename P = ident,
                CONCEPT_REQUIRES_(RandomAccessIterator<I>() && IteratorRange<I, S>() && Sortable<I, C, P>())>
            RANGES_RELAXED_CONSTEXPR
            I operator()(I begin, S end, C pred = C{}, P proj = P{}) const
            {
                auto n = distance(begin, end);
                detail::sift_up_n(begin, n, std::move(pred), std::move(proj));
                return begin + n;
            }

            template<typename Rng, typename C = ordered_less, typename P = ident,
                typename I = range_iterator_t<Rng>,
                CONCEPT_REQUIRES_(RandomAccessIterable<Rng>() && Sortable<I, C, P>())>
            RANGES_RELAXED_CONSTEXPR
            range_safe_iterator_t<Rng> operator()(Rng &&rng, C pred = C{}, P proj = P{}) const
            {
                I begin = ranges::begin(rng);
                auto n = distance(rng);
                detail::sift_up_n(begin, n, std::move(pred), std::move(proj));
                return begin + n;
            }
        };

        /// \sa `push_heap_fn`
        /// \ingroup group-algorithms
        namespace
        {
            constexpr auto&& push_heap = static_const<with_braced_init_args<push_heap_fn>>::value;
        }
        /// @}

        /// \cond
        namespace detail
        {
            struct pop_heap_n_fn
            {
                template<typename I, typename C = ordered_less, typename P = ident,
                    CONCEPT_REQUIRES_(RandomAccessIterator<I>() && Sortable<I, C, P>())>
                RANGES_RELAXED_CONSTEXPR
                void operator()(I begin, iterator_difference_t<I> len, C pred = C{},
                    P proj = P{}) const
                {
                    if(len > 1)
                    {
                        ranges::iter_swap(begin, begin + (len-1));
                        detail::sift_down_n(begin, len-1, begin, std::move(pred), std::move(proj));
                    }
                }
            };

            namespace
            {
                constexpr auto&& pop_heap_n = static_const<pop_heap_n_fn>::value;
            }
        }
        /// \endcond

        /// \addtogroup group-algorithms
        /// @{
        struct pop_heap_fn
        {
            template<typename I, typename S, typename C = ordered_less, typename P = ident,
                CONCEPT_REQUIRES_(RandomAccessIterator<I>() && IteratorRange<I, S>() && Sortable<I, C, P>())>
            RANGES_RELAXED_CONSTEXPR
            I operator()(I begin, S end, C pred = C{}, P proj = P{}) const
            {
                auto n = distance(begin, end);
                detail::pop_heap_n(begin, n, std::move(pred), std::move(proj));
                return begin + n;
            }

            template<typename Rng, typename C = ordered_less, typename P = ident,
                typename I = range_iterator_t<Rng>,
                CONCEPT_REQUIRES_(RandomAccessIterable<Rng>() && Sortable<I, C, P>())>
            RANGES_RELAXED_CONSTEXPR
            range_safe_iterator_t<Rng> operator()(Rng &&rng, C pred = C{}, P proj = P{}) const
            {
                I begin = ranges::begin(rng);
                auto n = distance(rng);
                detail::pop_heap_n(begin, n, std::move(pred), std::move(proj));
                return begin + n;
            }
        };

        /// \sa `pop_heap_fn`
        /// \ingroup group-algorithms
        namespace
        {
            constexpr auto&& pop_heap = static_const<with_braced_init_args<pop_heap_fn>>::value;
        }

        struct make_heap_fn
        {
            template<typename I, typename S, typename C = ordered_less, typename P = ident,
                CONCEPT_REQUIRES_(RandomAccessIterator<I>() && IteratorRange<I, S>() && Sortable<I, C, P>())>
            RANGES_RELAXED_CONSTEXPR
            I operator()(I begin, S end, C pred_ = C{}, P proj_ = P{}) const
            {
                auto &&pred = as_function(pred_);
                auto &&proj = as_function(proj_);
                iterator_difference_t<I> const n = distance(begin, end);
                if(n > 1)
                    // start from the first parent, there is no need to consider children
                    for(auto start = (n - 2) / 2; start >= 0; --start)
                        detail::sift_down_n(begin, n, begin + start, ranges::ref(pred), ranges::ref(proj));
                return begin + n;
            }

            template<typename Rng, typename C = ordered_less, typename P = ident,
                typename I = range_iterator_t<Rng>,
                CONCEPT_REQUIRES_(RandomAccessIterable<Rng>() && Sortable<I, C, P>())>
            RANGES_RELAXED_CONSTEXPR
            range_safe_iterator_t<Rng> operator()(Rng &&rng, C pred_ = C{}, P proj_ = P{}) const
            {
                auto &&pred = as_function(pred_);
                auto &&proj = as_function(proj_);
                I begin = ranges::begin(rng);
                iterator_difference_t<I> const n = distance(rng);
                if(n > 1)
                    // start from the first parent, there is no need to consider children
                    for(auto start = (n - 2) / 2; start >= 0; --start)
                        detail::sift_down_n(begin, n, begin + start, ranges::ref(pred), ranges::ref(proj));
                return begin + n;
            }
        };

        /// \sa `make_heap_fn`
        /// \ingroup group-algorithms
        namespace
        {
            constexpr auto&& make_heap = static_const<with_braced_init_args<make_heap_fn>>::value;
        }

        struct sort_heap_fn
        {
            template<typename I, typename S, typename C = ordered_less, typename P = ident,
                CONCEPT_REQUIRES_(RandomAccessIterator<I>() && IteratorRange<I, S>() && Sortable<I, C, P>())>
            I operator()(I begin, S end, C pred_ = C{}, P proj_ = P{}) const
            {
                auto &&pred = as_function(pred_);
                auto &&proj = as_function(proj_);
                iterator_difference_t<I> const n = distance(begin, end);
                for(auto i = n; i > 1; --i)
                    detail::pop_heap_n(begin, i, ranges::ref(pred), ranges::ref(proj));
                return begin + n;
            }

            template<typename Rng, typename C = ordered_less, typename P = ident,
                typename I = range_iterator_t<Rng>,
                CONCEPT_REQUIRES_(RandomAccessIterable<Rng &>() && Sortable<I, C, P>())>
            range_safe_iterator_t<Rng> operator()(Rng &&rng, C pred_ = C{}, P proj_ = P{}) const
            {
                auto &&pred = as_function(pred_);
                auto &&proj = as_function(proj_);
                I begin = ranges::begin(rng);
                iterator_difference_t<I> const n = distance(rng);
                for(auto i = n; i > 1; --i)
                    detail::pop_heap_n(begin, i, ranges::ref(pred), ranges::ref(proj));
                return begin + n;
            }
        };

        /// \sa `sort_heap_fn`
        /// \ingroup group-algorithms
        namespace
        {
            constexpr auto&& sort_heap = static_const<with_braced_init_args<sort_heap_fn>>::value;
        }

        /// @}
    } // namespace v3
} // namespace ranges

#endif // include guard
