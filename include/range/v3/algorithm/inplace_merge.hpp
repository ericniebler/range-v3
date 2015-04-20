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
#ifndef RANGES_V3_ALGORITHM_INPLACE_MERGE_HPP
#define RANGES_V3_ALGORITHM_INPLACE_MERGE_HPP

#include <new>
#include <memory>
#include <algorithm>
#include <functional>
#include <type_traits>
#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/distance.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/memory.hpp>
#include <range/v3/utility/iterator.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/swap.hpp>
#include <range/v3/algorithm/lower_bound.hpp>
#include <range/v3/algorithm/upper_bound.hpp>
#include <range/v3/algorithm/move.hpp>
#include <range/v3/algorithm/merge_move.hpp>
#include <range/v3/algorithm/rotate.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \cond
        namespace detail
        {
            struct merge_adaptive_fn
            {
                using buffered_t = std::true_type;
                static constexpr buffered_t buffered{};
                using unbuffered_t = std::false_type;
                static constexpr unbuffered_t unbuffered{};

            private:
                template<typename I, typename C, typename P>
                static void impl(buffered_t, I begin, I middle, I end, iterator_difference_t<I> len1,
                    iterator_difference_t<I> len2, iterator_value_t<I> *buf, C &pred, P &proj)
                {
                    using value_type = iterator_value_t<I>;
                    std::unique_ptr<value_type, detail::destroy_n<value_type>> h{buf, {}};
                    auto p = ranges::make_counted_raw_storage_iterator(buf, h.get_deleter());
                    if(len1 <= len2)
                    {
                        p = ranges::move(begin, middle, p).second;
                        merge_move(buf, p.base().base(), std::move(middle), std::move(end),
                            std::move(begin), std::ref(pred), std::ref(proj), std::ref(proj));
                    }
                    else
                    {
                        p = ranges::move(middle, end, p).second;
                        using RBi = std::reverse_iterator<I>;
                        using Rv = std::reverse_iterator<value_type*>;
                        merge_move(RBi{std::move(middle)}, RBi{std::move(begin)},
                            Rv{p.base().base()}, Rv{buf}, RBi{std::move(end)},
                            not_(std::ref(pred)), std::ref(proj), std::ref(proj));
                    }
                }
                template<typename I, typename C, typename P>
                RANGES_RELAXED_CONSTEXPR
                static void impl(unbuffered_t, I, I, I, iterator_difference_t<I>,
                    iterator_difference_t<I>, iterator_value_t<I>*, C &, P &)
                {}



            public:
                template<typename Impl, typename I, typename C = ordered_less, typename P = ident,
                    CONCEPT_REQUIRES_(BidirectionalIterator<I>() && Sortable<I, C, P>())>
                RANGES_RELAXED_CONSTEXPR
                void operator()(Impl, I begin, I middle, I end, iterator_difference_t<I> len1,
                    iterator_difference_t<I> len2, iterator_value_t<I> *buf,
                    std::ptrdiff_t buf_size, C pred_ = C{}, P proj_ = P{}) const
                {
                    using D = iterator_difference_t<I>;
                    auto &&pred = as_function(pred_);
                    auto &&proj = as_function(proj_);
                    while(true)
                    {
                        // if middle == end, we're done
                        if(len2 == 0)
                            return;
                        // shrink [begin, middle) as much as possible (with no moves), returning if it shrinks to 0
                        for(; true; ++begin, --len1)
                        {
                            if(len1 == 0)
                                return;
                            if(pred(proj(*middle), proj(*begin)))
                                break;
                        }
                        if(len1 <= buf_size || len2 <= buf_size)
                        {
                            merge_adaptive_fn::impl(Impl{}, std::move(begin), std::move(middle),
                                std::move(end), len1, len2, buf, pred, proj);
                            return;
                        }
                        // begin < middle < end
                        // *begin > *middle
                        // partition [begin, m1) [m1, middle) [middle, m2) [m2, end) such that
                        //     all elements in:
                        //         [begin, m1)  <= [middle, m2)
                        //         [middle, m2) <  [m1, middle)
                        //         [m1, middle) <= [m2, end)
                        //     and m1 or m2 is in the middle of its range
                        I m1 = I{};  // "median" of [begin, middle)
                        I m2 = I{};  // "median" of [middle, end)
                        D len11 = D{};      // distance(begin, m1)
                        D len21 = D{};      // distance(middle, m2)
                        // binary search smaller range
                        if(len1 < len2)
                        {   // len >= 1, len2 >= 2
                            len21 = len2 / 2;
                            m2 = next(middle, len21);
                            m1 = upper_bound(begin, middle, proj(*m2), ranges::ref(pred), ranges::ref(proj));
                            len11 = distance(begin, m1);
                        }
                        else
                        {
                            if(len1 == 1)
                            {   // len1 >= len2 && len2 > 0, therefore len2 == 1
                                // It is known *begin > *middle
                                ranges::iter_swap(begin, middle);
                                return;
                            }
                            // len1 >= 2, len2 >= 1
                            len11 = len1 / 2;
                            m1 = next(begin, len11);
                            m2 = lower_bound(middle, end, proj(*m1), ranges::ref(pred), ranges::ref(proj));
                            len21 = distance(middle, m2);
                        }
                        D len12 = len1 - len11;  // distance(m1, middle)
                        D len22 = len2 - len21;  // distance(m2, end)
                        // [begin, m1) [m1, middle) [middle, m2) [m2, end)
                        // swap middle two partitions
                        middle = rotate(m1, std::move(middle), m2).begin();
                        // len12 and len21 now have swapped meanings
                        // merge smaller range with recursive call and larger with tail recursion elimination
                        if(len11 + len21 < len12 + len22)
                        {
                            (*this)(Impl{}, std::move(begin), std::move(m1), middle, len11, len21, buf, buf_size,
                                ranges::ref(pred), ranges::ref(proj));
                            begin = std::move(middle);
                            middle = std::move(m2);
                            len1 = len12;
                            len2 = len22;
                        }
                        else
                        {
                            (*this)(Impl{}, middle, std::move(m2), std::move(end), len12, len22, buf, buf_size,
                                ranges::ref(pred), ranges::ref(proj));
                            end = std::move(middle);
                            middle = std::move(m1);
                            len1 = len11;
                            len2 = len21;
                        }
                    }
                }
                template<typename I, typename C = ordered_less, typename P = ident,
                         CONCEPT_REQUIRES_(BidirectionalIterator<I>() && Sortable<I, C, P>())>
                void operator()(I begin, I middle, I end, iterator_difference_t<I> len1,
                                iterator_difference_t<I> len2, iterator_value_t<I> *buf,
                                std::ptrdiff_t buf_size, C pred_ = C{}, P proj_ = P{}) const
                {
                    (*this)(buffered, std::move(begin), std::move(middle),
                            std::move(end), len1, len2, buf, buf_size,
                            std::move(pred_), std::move(proj_));
                }
            };

            namespace
            {
                constexpr auto&& merge_adaptive = static_const<merge_adaptive_fn>::value;
            }

            struct inplace_merge_no_buffer_fn
            {
                template<typename I, typename C = ordered_less, typename P = ident,
                    CONCEPT_REQUIRES_(BidirectionalIterator<I>() && Sortable<I, C, P>())>
                RANGES_RELAXED_CONSTEXPR
                I operator()(I begin, I middle, I end, C pred = C{}, P proj = P{}) const
                {
                    auto len1 = distance(begin, middle);
                    auto len2_and_end = enumerate(middle, end);
                    merge_adaptive(merge_adaptive.unbuffered,
                                   std::move(begin), std::move(middle),
                                   len2_and_end.second,
                                   len1, len2_and_end.first,
                        nullptr, 0, std::move(pred), std::move(proj));
                    return len2_and_end.second;
                }

                template<typename Rng, typename C = ordered_less, typename P = ident,
                         typename I = range_iterator_t<Rng>,
                         CONCEPT_REQUIRES_(BidirectionalIterable<Rng>() && Sortable<I, C, P>())>
                RANGES_RELAXED_CONSTEXPR
                range_safe_iterator_t<Rng>
                operator()(Rng &&rng, I middle, C pred = C{}, P proj = P{}) const
                {
                    return (*this)(begin(rng), std::move(middle), end(rng), std::move(pred),
                                   std::move(proj));
                }
            };

            namespace
            {
                constexpr auto&& inplace_merge_no_buffer = static_const<inplace_merge_no_buffer_fn>::value;
            }
        }
        /// \endcond

        /// \addtogroup group-algorithms
        /// @{
        struct inplace_merge_fn
        {
            // TODO reimplement to only need forward iterators
            template<typename I, typename S, typename C = ordered_less, typename P = ident,
                CONCEPT_REQUIRES_(BidirectionalIterator<I>() && Sortable<I, C, P>())>
            I operator()(I begin, I middle, S end, C pred = C{}, P proj = P{}) const
            {
                using value_type = iterator_value_t<I>;
                auto len1 = distance(begin, middle);
                auto len2_and_end = enumerate(middle, end);
                auto buf_size = std::min(len1, len2_and_end.first);
                std::pair<value_type*, std::ptrdiff_t> buf{nullptr, 0};
                std::unique_ptr<value_type, detail::return_temporary_buffer> h;
                if(detail::is_trivially_copy_assignable<value_type>::value && 8 < buf_size)
                {
                    buf = std::get_temporary_buffer<value_type>(buf_size);
                    h.reset(buf.first);
                }
                detail::merge_adaptive(detail::merge_adaptive.buffered,
                    std::move(begin), std::move(middle), len2_and_end.second,
                    len1, len2_and_end.first, buf.first, buf.second, std::move(pred),
                    std::move(proj));
                return len2_and_end.second;
            }

            template<typename Rng, typename C = ordered_less, typename P = ident,
                typename I = range_iterator_t<Rng>,
                CONCEPT_REQUIRES_(BidirectionalIterable<Rng>() && Sortable<I, C, P>())>
            RANGES_RELAXED_CONSTEXPR
            range_safe_iterator_t<Rng>
            operator()(Rng &&rng, I middle, C pred = C{}, P proj = P{}) const
            {
                return (*this)(begin(rng), std::move(middle), end(rng), std::move(pred),
                    std::move(proj));
            }
        };

        /// \sa `inplace_merge_fn`
        /// \ingroup group-algorithms
        namespace
        {
            constexpr auto&& inplace_merge = static_const<with_braced_init_args<inplace_merge_fn>>::value;
        }

        /// @}
    } // namespace v3
} // namespace ranges

#endif // include guard
