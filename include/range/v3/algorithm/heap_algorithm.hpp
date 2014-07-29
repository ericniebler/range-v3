//  Copyright Eric Niebler 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See acpredanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
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
#include <range/v3/utility/range_algorithm.hpp>

namespace ranges
{
    inline namespace v3
    {
        template<typename I, typename C = ordered_less, typename P = ident,
            typename V = iterator_value_t<I>,
            typename X = concepts::Invokable::result_t<P, V>>
        constexpr bool IsHeapable()
        {
            return RandomAccessIterator<I>() && Invokable<P, V>() && InvokableRelation<C, X>();
        }

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
                    return begin + n_;
                }
            };

            RANGES_CONSTEXPR range_algorithm<is_heap_until_n_fn> is_heap_until_n {};

            struct is_heap_n_fn
            {
                template<typename I, typename C = ordered_less, typename P = ident,
                    CONCEPT_REQUIRES_(IsHeapable<I, C, P>())>
                bool operator()(I begin, iterator_difference_t<I> n, C pred = C{}, P proj = P{}) const
                {
                    return is_heap_until_n(begin, n, std::move(pred), std::move(proj)) == begin + n;
                }
            };

            RANGES_CONSTEXPR range_algorithm<is_heap_n_fn> is_heap_n {};
        }

        struct is_heap_until_fn
        {
            template<typename I, typename S, typename C = ordered_less, typename P = ident,
                CONCEPT_REQUIRES_(IsHeapable<I, C, P>() && Sentinel<S, I>())>
            I operator()(I begin, S end, C pred = C{}, P proj = P{}) const
            {
                return detail::is_heap_until_n(std::move(begin), distance(begin, end), std::move(pred),
                    std::move(proj));
            }

            template<typename Rng, typename C = ordered_less, typename P = ident,
                typename I = range_iterator_t<Rng>,
                CONCEPT_REQUIRES_(IsHeapable<I, C, P>() && Iterable<Rng>())>
            I operator()(Rng &rng, C pred = C{}, P proj = P{}) const
            {
                return detail::is_heap_until_n(begin(rng), distance(rng), std::move(pred), std::move(proj));
            }
        };

        RANGES_CONSTEXPR range_algorithm<is_heap_until_fn> is_heap_until {};

        struct is_heap_fn
        {
            template<typename I, typename S, typename C = ordered_less, typename P = ident,
                CONCEPT_REQUIRES_(IsHeapable<I, C, P>() && Sentinel<S, I>())>
            bool operator()(I begin, S end, C pred = C{}, P proj = P{}) const
            {
                return is_heap_n(std::move(begin), distance(begin, end), std::move(pred),
                    std::move(proj));
            }

            template<typename Rng, typename C = ordered_less, typename P = ident,
                typename I = range_iterator_t<Rng>,
                CONCEPT_REQUIRES_(IsHeapable<I, C, P>() && Iterable<Rng>())>
            bool operator()(Rng &&rng, C pred = C{}, P proj = P{}) const
            {
                return is_heap_n(begin(rng), distance(rng), std::move(pred), std::move(proj));
            }
        };

        RANGES_CONSTEXPR range_algorithm<is_heap_fn> is_heap {};

        namespace detail
        {
            struct push_heap_front_n_fn
            {
                template<typename I, typename C = ordered_less, typename P = ident,
                    CONCEPT_REQUIRES_(RandomAccessIterator<I>() && Sortable<I, C, P>())>
                void operator()(I begin, iterator_difference_t<I> len, C pred_ = C{},
                    P proj_ = P{}) const
                {
                    auto &&pred = invokable(pred_);
                    auto &&proj = invokable(proj_);
                    if(len > 1)
                    {
                        iterator_difference_t<I> p = 0, c = 2;
                        I pp = begin, cp = begin + c;
                        if(c == len || pred(proj(*cp), proj(*(cp - 1))))
                        {
                            --c;
                            --cp;
                        }
                        if(pred(proj(*pp), proj(*cp)))
                        {
                            iterator_value_t<I> t(std::move(*pp));
                            do
                            {
                                *pp = std::move(*cp);
                                pp = cp;
                                p = c;
                                c = (p + 1) * 2;
                                if(c > len)
                                    break;
                                cp = begin + c;
                                if(c == len || pred(proj(*cp), proj(*(cp - 1))))
                                {
                                    --c;
                                    --cp;
                                }
                            } while(pred(proj(t), proj(*cp)));
                            *pp = std::move(t);
                        }
                    }
                }
            };

            RANGES_CONSTEXPR push_heap_front_n_fn push_heap_front_n{};

            struct push_heap_back_n_fn
            {
                template<typename I, typename C = ordered_less, typename P = ident,
                    CONCEPT_REQUIRES_(RandomAccessIterator<I>() && Sortable<I, C, P>())>
                void operator()(I begin, iterator_difference_t<I> len, C pred_ = C{},
                    P proj_ = P{}) const
                {
                    auto &&pred = invokable(pred_);
                    auto &&proj = invokable(proj_);
                    I end = begin + len;
                    if(len > 1)
                    {
                        len = (len - 2) / 2;
                        I ptr = begin + len;
                        if(pred(proj(*ptr), proj(*--end)))
                        {
                            auto t(std::move(*end));
                            do
                            {
                                *end = std::move(*ptr);
                                end = ptr;
                                if(len == 0)
                                    break;
                                len = (len - 1) / 2;
                                ptr = begin + len;
                            } while(pred(proj(*ptr), proj(t)));
                            *end = std::move(t);
                        }
                    }
                }
            };

            RANGES_CONSTEXPR push_heap_back_n_fn push_heap_back_n{};
        }

        struct push_heap_fn
        {
            template<typename I, typename S, typename C = ordered_less, typename P = ident,
                CONCEPT_REQUIRES_(RandomAccessIterator<I, S>() && Sortable<I, C, P>())>
            I operator()(I begin, S end, C pred = C{}, P proj = P{}) const
            {
                auto n = distance(begin, end);
                detail::push_heap_back_n(begin, n, std::move(pred), std::move(proj));
                return begin + n;
            }

            template<typename Rng, typename C = ordered_less, typename P = ident,
                typename I = range_iterator_t<Rng>,
                CONCEPT_REQUIRES_(RandomAccessIterable<Rng>() && Sortable<I, C, P>())>
            I operator()(Rng & rng, C pred = C{}, P proj = P{}) const
            {
                I begin = ranges::begin(rng);
                auto n = distance(rng);
                detail::push_heap_back_n(begin, n, std::move(pred), std::move(proj));
                return begin + n;
            }
        };

        RANGES_CONSTEXPR push_heap_fn push_heap {};

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
                        ranges::swap(*begin, *(begin + (len-1)));
                        detail::push_heap_front_n(std::move(begin), len-1, std::move(pred),
                            std::move(proj));
                    }
                }
            };

            RANGES_CONSTEXPR pop_heap_n_fn pop_heap_n{};
        }

        struct pop_heap_fn
        {
            template<typename I, typename S, typename C = ordered_less, typename P = ident,
                CONCEPT_REQUIRES_(RandomAccessIterator<I, S>() && Sortable<I, C, P>())>
            I operator()(I begin, S end, C pred = C{}, P proj = P{}) const
            {
                auto n = distance(begin, end);
                detail::pop_heap_n(begin, n, std::move(pred), std::move(proj));
                return begin + n;
            }

            template<typename Rng, typename C = ordered_less, typename P = ident,
                typename I = range_iterator_t<Rng>,
                CONCEPT_REQUIRES_(RandomAccessIterable<Rng>() && Sortable<I, C, P>())>
            I operator()(Rng & rng, C pred = C{}, P proj = P{}) const
            {
                I begin = ranges::begin(rng);
                auto n = distance(rng);
                detail::pop_heap_n(begin, n, std::move(pred), std::move(proj));
                return begin + n;
            }
        };

        RANGES_CONSTEXPR pop_heap_fn pop_heap {};

        struct make_heap_fn
        {
            template<typename I, typename S, typename C = ordered_less, typename P = ident,
                CONCEPT_REQUIRES_(RandomAccessIterator<I, S>() && Sortable<I, C, P>())>
            I operator()(I begin, S end, C pred_ = C{}, P proj_ = P{}) const
            {
                auto &&pred = invokable(pred_);
                auto &&proj = invokable(proj_);
                iterator_difference_t<I> const n = distance(begin, end);
                for(iterator_difference_t<I> i = 1; i < n;)
                    detail::push_heap_back_n(begin, ++i, std::ref(pred), std::ref(proj));
                return begin + n;
            }

            template<typename Rng, typename C = ordered_less, typename P = ident,
                typename I = range_iterator_t<Rng>,
                CONCEPT_REQUIRES_(RandomAccessIterable<Rng>() && Sortable<I, C, P>())>
            I operator()(Rng & rng, C pred_ = C{}, P proj_ = P{}) const
            {
                auto &&pred = invokable(pred_);
                auto &&proj = invokable(proj_);
                I begin = ranges::begin(rng);
                iterator_difference_t<I> const n = distance(rng);
                for(iterator_difference_t<I> i = 1; i < n;)
                    detail::push_heap_back_n(begin, ++i, std::ref(pred), std::ref(proj));
                return begin + n;
            }
        };

        RANGES_CONSTEXPR make_heap_fn make_heap {};

        struct sort_heap_fn
        {
            template<typename I, typename S, typename C = ordered_less, typename P = ident,
                CONCEPT_REQUIRES_(RandomAccessIterator<I, S>() && Sortable<I, C, P>())>
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
                CONCEPT_REQUIRES_(RandomAccessIterable<Rng>() && Sortable<I, C, P>())>
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

        RANGES_CONSTEXPR sort_heap_fn sort_heap {};

    } // namespace v3
} // namespace ranges

#endif // include guard
