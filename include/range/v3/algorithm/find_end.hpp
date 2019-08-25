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
#ifndef RANGES_V3_ALGORITHM_FIND_END_HPP
#define RANGES_V3_ALGORITHM_FIND_END_HPP

#include <utility>

#include <meta/meta.hpp>

#include <range/v3/range_fwd.hpp>

#include <range/v3/functional/comparisons.hpp>
#include <range/v3/functional/identity.hpp>
#include <range/v3/functional/invoke.hpp>
#include <range/v3/iterator/concepts.hpp>
#include <range/v3/iterator/operations.hpp>
#include <range/v3/iterator/traits.hpp>
#include <range/v3/range/access.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/range/traits.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/view/subrange.hpp>

namespace ranges
{
    /// \cond
    namespace detail
    {
        template<typename I, typename S>
        auto next_to_if(I i, S s, std::true_type) -> CPP_ret(I)( //
            requires input_iterator<I> && sentinel_for<S, I>)
        {
            return ranges::next(i, s);
        }

        template<typename I, typename S>
        auto next_to_if(I, S s, std::false_type) -> CPP_ret(S)( //
            requires input_iterator<I> && sentinel_for<S, I>)
        {
            return s;
        }

        template<bool B, typename I, typename S>
        auto next_to_if(I i, S s) -> CPP_ret(meta::if_c<B, I, S>)( //
            requires input_iterator<I> && sentinel_for<S, I>)
        {
            return detail::next_to_if(std::move(i), std::move(s), meta::bool_<B>{});
        }

        template<typename I1, typename S1, typename I2, typename S2, typename R,
                 typename P>
        subrange<I1> find_end_impl(I1 begin1, S1 end1, I2 begin2, S2 end2, R pred, P proj,
                                   detail::forward_iterator_tag_,
                                   detail::forward_iterator_tag_)
        {
            bool found = false;
            I1 res_begin, res_end;
            if(begin2 == end2)
            {
                auto e1 = ranges::next(begin1, end1);
                return {e1, e1};
            }
            while(true)
            {
                while(true)
                {
                    if(begin1 == end1)
                        return {(found ? res_begin : begin1), (found ? res_end : begin1)};
                    if(invoke(pred, invoke(proj, *begin1), *begin2))
                        break;
                    ++begin1;
                }
                auto tmp1 = begin1;
                auto tmp2 = begin2;
                while(true)
                {
                    if(++tmp2 == end2)
                    {
                        res_begin = begin1++;
                        res_end = ++tmp1;
                        found = true;
                        break;
                    }
                    if(++tmp1 == end1)
                        return {(found ? res_begin : tmp1), (found ? res_end : tmp1)};
                    if(!invoke(pred, invoke(proj, *tmp1), *tmp2))
                    {
                        ++begin1;
                        break;
                    }
                }
            }
        }

        template<typename I1, typename I2, typename R, typename P>
        subrange<I1> find_end_impl(I1 begin1, I1 end1, I2 begin2, I2 end2, R pred, P proj,
                                   detail::bidirectional_iterator_tag_,
                                   detail::bidirectional_iterator_tag_)
        {
            // modeled after search algorithm (in reverse)
            if(begin2 == end2)
                return {end1, end1}; // Everything matches an empty sequence
            I1 l1 = end1;
            I2 l2 = end2;
            --l2;
            while(true)
            {
                // Find end element in sequence 1 that matches *(end2-1), with a mininum
                // of loop checks
                do
                    // return {end1,end1} if no element matches *begin2
                    if(begin1 == l1)
                        return {end1, end1};
                while(!invoke(pred, invoke(proj, *--l1), *l2));
                // *l1 matches *l2, now match elements before here
                I1 m1 = l1;
                I2 m2 = l2;
                do
                    // If pattern exhausted, {m1,++l1} is the answer
                    // (works for 1 element pattern)
                    if(m2 == begin2)
                        return {m1, ++l1};
                    // Otherwise if source exhausted, pattern not found
                    else if(m1 == begin1)
                        return {end1, end1};
                // if there is a mismatch, restart with a new l1
                // else there is a match, check next elements
                while(invoke(pred, invoke(proj, *--m1), *--m2));
            }
        }

        template<typename I1, typename I2, typename R, typename P>
        subrange<I1> find_end_impl(I1 begin1, I1 end1, I2 begin2, I2 end2, R pred, P proj,
                                   detail::random_access_iterator_tag_,
                                   detail::random_access_iterator_tag_)
        {
            // Take advantage of knowing source and pattern lengths.  Stop short when
            // source is smaller than pattern
            auto len2 = end2 - begin2;
            if(len2 == 0)
                return {end1, end1};
            auto len1 = end1 - begin1;
            if(len1 < len2)
                return {end1, end1};
            I1 const start =
                begin1 + (len2 - 1); // End of pattern match can't go before here
            I1 l1 = end1;
            I2 l2 = end2;
            --l2;
            while(true)
            {
                do
                    if(start == l1)
                        return {end1, end1};
                while(!invoke(pred, invoke(proj, *--l1), *l2));
                I1 m1 = l1;
                I2 m2 = l2;
                do
                    if(m2 == begin2)
                        return {m1, ++l1};
                // no need to check range on m1 because s guarantees we have enough source
                while(invoke(pred, invoke(proj, *--m1), *--m2));
            }
        }
    } // namespace detail
    /// \endcond

    /// \addtogroup group-algorithms
    /// @{
    RANGES_BEGIN_NIEBLOID(find_end)

        /// \brief function template \c find_end
        template<typename I1,
                 typename S1,
                 typename I2,
                 typename S2,
                 typename R = equal_to,
                 typename P = identity>
        auto RANGES_FUN_NIEBLOID(find_end)(
            I1 begin1, S1 end1, I2 begin2, S2 end2, R pred = R{}, P proj = P{}) //
            ->CPP_ret(subrange<I1>)(                                            //
                requires forward_iterator<I1> && sentinel_for<S1, I1> &&
                forward_iterator<I2> && sentinel_for<S2, I2> &&
                indirect_relation<R, projected<I1, P>, I2>)
        {
            constexpr bool Bidi =
                bidirectional_iterator<I1> && bidirectional_iterator<I2>;
            return detail::find_end_impl(begin1,
                                         detail::next_to_if<Bidi>(begin1, end1),
                                         begin2,
                                         detail::next_to_if<Bidi>(begin2, end2),
                                         std::move(pred),
                                         std::move(proj),
                                         iterator_tag_of<I1>(),
                                         iterator_tag_of<I2>());
        }

        /// \overload
        template<typename Rng1,
                 typename Rng2,
                 typename R = equal_to,
                 typename P = identity>
        auto RANGES_FUN_NIEBLOID(find_end)(
            Rng1 && rng1, Rng2 && rng2, R pred = R{}, P proj = P{}) //
            ->CPP_ret(safe_subrange_t<Rng1>)(                       //
                requires forward_range<Rng1> && forward_range<Rng2> &&
                indirect_relation<R, projected<iterator_t<Rng1>, P>, iterator_t<Rng2>>)
        {
            return (*this)(begin(rng1),
                           end(rng1),
                           begin(rng2),
                           end(rng2),
                           std::move(pred),
                           std::move(proj));
        }

    RANGES_END_NIEBLOID(find_end)

    namespace cpp20
    {
        using ranges::find_end;
    }
    /// @}
} // namespace ranges

#endif // include guard
