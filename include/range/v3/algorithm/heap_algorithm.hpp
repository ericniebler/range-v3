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
#include <range/v3/range_fwd.hpp>
#include <range/v3/distance.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/iterator.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/invokable.hpp>
#include <range/v3/utility/functional.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \ingroup group-concepts
        template<typename I, typename C = ordered_less, typename P = ident,
            typename V = iterator_value_t<I>,
            typename X = concepts::Invokable::result_t<P, V>>
        using IsHeapable = meta::fast_and<
            RandomAccessIterator<I>,
            Invokable<P, V>,
            InvokableRelation<C, X>>;

        /// \cond
        namespace detail
        {
            struct is_heap_until_n_fn
            {
                template<typename I, typename C = ordered_less, typename P = ident,
                    CONCEPT_REQUIRES_(IsHeapable<I, C, P>())>
                I operator()(I const begin_, iterator_difference_t<I> const n_, C pred_ = C{}, P proj_ = P{}) const
                {
                    RANGES_ASSERT(0 <= n_);
                    auto &&pred = invokable(pred_);
                    auto &&proj = invokable(proj_);
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

            constexpr is_heap_until_n_fn is_heap_until_n {};

            struct is_heap_n_fn
            {
                template<typename I, typename C = ordered_less, typename P = ident,
                    CONCEPT_REQUIRES_(IsHeapable<I, C, P>())>
                bool operator()(I begin, iterator_difference_t<I> n, C pred = C{}, P proj = P{}) const
                {
                    return is_heap_until_n(begin, n, std::move(pred), std::move(proj)) == begin + n;
                }
            };

            constexpr is_heap_n_fn is_heap_n {};
        }
        /// \endcond

        /// \addtogroup group-algorithms
        /// @{
        struct is_heap_until_fn
        {
            template<typename I, typename S, typename C = ordered_less, typename P = ident,
                CONCEPT_REQUIRES_(IsHeapable<I, C, P>() && IteratorRange<I, S>())>
            I operator()(I begin, S end, C pred = C{}, P proj = P{}) const
            {
                return detail::is_heap_until_n(std::move(begin), distance(begin, end), std::move(pred),
                    std::move(proj));
            }

            template<typename Rng, typename C = ordered_less, typename P = ident,
                typename I = range_iterator_t<Rng>,
                CONCEPT_REQUIRES_(IsHeapable<I, C, P>() && Iterable<Rng &>())>
            I operator()(Rng &rng, C pred = C{}, P proj = P{}) const
            {
                return detail::is_heap_until_n(begin(rng), distance(rng), std::move(pred), std::move(proj));
            }
        };

        /// \sa `is_heap_until_fn`
        /// \ingroup group-algorithms
        constexpr is_heap_until_fn is_heap_until {};

        struct is_heap_fn
        {
            template<typename I, typename S, typename C = ordered_less, typename P = ident,
                CONCEPT_REQUIRES_(IsHeapable<I, C, P>() && IteratorRange<I, S>())>
            bool operator()(I begin, S end, C pred = C{}, P proj = P{}) const
            {
                return detail::is_heap_n(std::move(begin), distance(begin, end), std::move(pred),
                    std::move(proj));
            }

            template<typename Rng, typename C = ordered_less, typename P = ident,
                typename I = range_iterator_t<Rng>,
                CONCEPT_REQUIRES_(IsHeapable<I, C, P>() && Iterable<Rng>())>
            bool operator()(Rng &&rng, C pred = C{}, P proj = P{}) const
            {
                return detail::is_heap_n(begin(rng), distance(rng), std::move(pred), std::move(proj));
            }
        };

        /// \sa `is_heap_fn`
        /// \ingroup group-algorithms
        constexpr with_braced_init_args<is_heap_fn> is_heap {};
        /// @}

        /// \cond
        namespace detail
        {
            struct sift_up_n_fn
            {
                template<typename I, typename C = ordered_less, typename P = ident>
                void operator()(I begin, iterator_difference_t<I> len, C pred_ = C{}, P proj_ = P{}) const
                {
                    if(len > 1)
                    {
                        auto &&pred = invokable(pred_);
                        auto &&proj = invokable(proj_);
                        I end = begin + len;
                        len = (len - 2) / 2;
                        I i = begin + len;
                        if(pred(proj(*i), proj(*--end)))
                        {
                            iterator_value_t<I> v(std::move(*end));
                            do
                            {
                                *end = std::move(*i);
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

            constexpr sift_up_n_fn sift_up_n{};

            struct sift_down_n_fn
            {
                template<typename I, typename C = ordered_less, typename P = ident>
                void operator()(I begin, iterator_difference_t<I> len, I start, C pred_ = C {}, P proj_ = P{}) const
                {
                    // left-child of start is at 2 * start + 1
                    // right-child of start is at 2 * start + 2
                    auto child = start - begin;

                    if(len < 2 || (len - 2) / 2 < child)
                        return;

                    child = 2 * child + 1;
                    I child_i = begin + child;

                    auto &&pred = invokable(pred_);
                    auto &&proj = invokable(proj_);

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

                    iterator_value_t<I> top(std::move(*start));
                    do
                    {
                        // we are not in heap-order, swap the parent with it's largest child
                        *start = std::move(*child_i);
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

            constexpr sift_down_n_fn sift_down_n{};
        }
        /// \endcond

        /// \addtogroup group-algorithms
        /// @{
        struct push_heap_fn
        {
            template<typename I, typename S, typename C = ordered_less, typename P = ident,
                CONCEPT_REQUIRES_(RandomAccessIterator<I>() && IteratorRange<I, S>() && Sortable<I, C, P>())>
            I operator()(I begin, S end, C pred = C{}, P proj = P{}) const
            {
                auto n = distance(begin, end);
                detail::sift_up_n(begin, n, std::move(pred), std::move(proj));
                return begin + n;
            }

            template<typename Rng, typename C = ordered_less, typename P = ident,
                typename I = range_iterator_t<Rng>,
                CONCEPT_REQUIRES_(RandomAccessIterable<Rng &>() && Sortable<I, C, P>())>
            I operator()(Rng & rng, C pred = C{}, P proj = P{}) const
            {
                I begin = ranges::begin(rng);
                auto n = distance(rng);
                detail::sift_up_n(begin, n, std::move(pred), std::move(proj));
                return begin + n;
            }
        };

        /// \sa `push_heap_fn`
        /// \ingroup group-algorithms
        constexpr push_heap_fn push_heap {};
        /// @}

        /// \cond
        namespace detail
        {
            struct pop_heap_n_fn
            {
                template<typename I, typename C = ordered_less, typename P = ident,
                    CONCEPT_REQUIRES_(RandomAccessIterator<I>() && Sortable<I, C, P>())>
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

            constexpr pop_heap_n_fn pop_heap_n{};
        }
        /// \endcond

        /// \addtogroup group-algorithms
        /// @{
        struct pop_heap_fn
        {
            template<typename I, typename S, typename C = ordered_less, typename P = ident,
                CONCEPT_REQUIRES_(RandomAccessIterator<I>() && IteratorRange<I, S>() && Sortable<I, C, P>())>
            I operator()(I begin, S end, C pred = C{}, P proj = P{}) const
            {
                auto n = distance(begin, end);
                detail::pop_heap_n(begin, n, std::move(pred), std::move(proj));
                return begin + n;
            }

            template<typename Rng, typename C = ordered_less, typename P = ident,
                typename I = range_iterator_t<Rng>,
                CONCEPT_REQUIRES_(RandomAccessIterable<Rng &>() && Sortable<I, C, P>())>
            I operator()(Rng & rng, C pred = C{}, P proj = P{}) const
            {
                I begin = ranges::begin(rng);
                auto n = distance(rng);
                detail::pop_heap_n(begin, n, std::move(pred), std::move(proj));
                return begin + n;
            }
        };

        /// \sa `pop_heap_fn`
        /// \ingroup group-algorithms
        constexpr pop_heap_fn pop_heap {};

        struct make_heap_fn
        {
            template<typename I, typename S, typename C = ordered_less, typename P = ident,
                CONCEPT_REQUIRES_(RandomAccessIterator<I>() && IteratorRange<I, S>() && Sortable<I, C, P>())>
            I operator()(I begin, S end, C pred_ = C{}, P proj_ = P{}) const
            {
                auto &&pred = invokable(pred_);
                auto &&proj = invokable(proj_);
                iterator_difference_t<I> const n = distance(begin, end);
                if(n > 1)
                    // start from the first parent, there is no need to consider children
                    for(auto start = (n - 2) / 2; start >= 0; --start)
                        detail::sift_down_n(begin, n, begin + start, std::ref(pred), std::ref(proj));
                return begin + n;
            }

            template<typename Rng, typename C = ordered_less, typename P = ident,
                typename I = range_iterator_t<Rng>,
                CONCEPT_REQUIRES_(RandomAccessIterable<Rng &>() && Sortable<I, C, P>())>
            I operator()(Rng & rng, C pred_ = C{}, P proj_ = P{}) const
            {
                auto &&pred = invokable(pred_);
                auto &&proj = invokable(proj_);
                I begin = ranges::begin(rng);
                iterator_difference_t<I> const n = distance(rng);
                if(n > 1)
                    // start from the first parent, there is no need to consider children
                    for(auto start = (n - 2) / 2; start >= 0; --start)
                        detail::sift_down_n(begin, n, begin + start, std::ref(pred), std::ref(proj));
                return begin + n;
            }
        };

        /// \sa `make_heap_fn`
        /// \ingroup group-algorithms
        constexpr make_heap_fn make_heap {};

        struct sort_heap_fn
        {
            template<typename I, typename S, typename C = ordered_less, typename P = ident,
                CONCEPT_REQUIRES_(RandomAccessIterator<I>() && IteratorRange<I, S>() && Sortable<I, C, P>())>
            I operator()(I begin, S end, C pred_ = C{}, P proj_ = P{}) const
            {
                auto &&pred = invokable(pred_);
                auto &&proj = invokable(proj_);
                iterator_difference_t<I> const n = distance(begin, end);
                for(auto i = n; i > 1; --i)
                    detail::pop_heap_n(begin, i, std::ref(pred), std::ref(proj));
                return begin + n;
            }

            template<typename Rng, typename C = ordered_less, typename P = ident,
                typename I = range_iterator_t<Rng>,
                CONCEPT_REQUIRES_(RandomAccessIterable<Rng &>() && Sortable<I, C, P>())>
            I operator()(Rng & rng, C pred_ = C{}, P proj_ = P{}) const
            {
                auto &&pred = invokable(pred_);
                auto &&proj = invokable(proj_);
                I begin = ranges::begin(rng);
                iterator_difference_t<I> const n = distance(rng);
                for(auto i = n; i > 1; --i)
                    detail::pop_heap_n(begin, i, std::ref(pred), std::ref(proj));
                return begin + n;
            }
        };

        /// \sa `sort_heap_fn`
        /// \ingroup group-algorithms
        constexpr sort_heap_fn sort_heap {};

        /// @}
    } // namespace v3
} // namespace ranges

#endif // include guard
