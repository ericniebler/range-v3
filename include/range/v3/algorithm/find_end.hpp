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
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \cond
        namespace detail
        {
            template<typename I, typename S,
                CONCEPT_REQUIRES_(InputIterator<I>() && Sentinel<S, I>())>
            I next_to_if(I i, S s, std::true_type)
            {
                return ranges::next(i, s);
            }

            template<typename I, typename S,
                CONCEPT_REQUIRES_(InputIterator<I>() && Sentinel<S, I>())>
            S next_to_if(I, S s, std::false_type)
            {
                return s;
            }

            template<bool B, typename I, typename S,
                CONCEPT_REQUIRES_(InputIterator<I>() && Sentinel<S, I>())>
            meta::if_c<B, I, S> next_to_if(I i, S s)
            {
                return detail::next_to_if(std::move(i), std::move(s), meta::bool_<B>{});
            }
        }
        /// \endcond

        /// \addtogroup group-algorithms
        /// @{
        struct find_end_fn
        {
        private:
            template<typename I1, typename S1, typename I2, typename S2, typename R, typename P>
            static I1
            impl(I1 begin1, S1 end1, I2 begin2, S2 end2, R pred, P proj,
                 concepts::ForwardIterator*, concepts::ForwardIterator*)
            {
                bool found = false;
                I1 res;
                if(begin2 == end2)
                    return ranges::next(begin1, end1);
                while(true)
                {
                    while(true)
                    {
                        if(begin1 == end1)
                            return found ? res : begin1;
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
                            res = begin1++;
                            found = true;
                            break;
                        }
                        if(++tmp1 == end1)
                            return found ? res : tmp1;
                        if(!invoke(pred, invoke(proj, *tmp1), *tmp2))
                        {
                            ++begin1;
                            break;
                        }
                    }
                }
            }

            template<typename I1, typename I2, typename R, typename P>
            static I1
            impl(I1 begin1, I1 end1, I2 begin2, I2 end2, R pred, P proj,
                 concepts::BidirectionalIterator*, concepts::BidirectionalIterator*)
            {
                // modeled after search algorithm (in reverse)
                if(begin2 == end2)
                    return end1;  // Everything matches an empty sequence
                I1 l1 = end1;
                I2 l2 = end2;
                --l2;
                while(true)
                {
                    // Find end element in sequence 1 that matches *(end2-1), with a mininum of loop checks
                        // return end1 if no element matches *begin2
                    do  if(begin1 == l1) return end1;
                    while(!invoke(pred, invoke(proj, *--l1), *l2));
                    // *l1 matches *l2, now match elements before here
                    I1 m1 = l1;
                    I2 m2 = l2;
                        // If pattern exhausted, m1 is the answer (works for 1 element pattern)
                    do  if(m2 == begin2) return m1;
                        // Otherwise if source exhausted, pattern not found
                        else if(m1 == begin1) return end1;
                        // if there is a mismatch, restart with a new l1
                        // else there is a match, check next elements
                    while(invoke(pred, invoke(proj, *--m1), *--m2));
                }
            }

            template<typename I1, typename I2, typename R, typename P>
            static I1
            impl(I1 begin1, I1 end1, I2 begin2, I2 end2, R pred, P proj,
                 concepts::RandomAccessIterator*, concepts::RandomAccessIterator*)
            {
                // Take advantage of knowing source and pattern lengths.  Stop short when source is smaller than pattern
                auto len2 = end2 - begin2;
                if(len2 == 0)
                    return end1;
                auto len1 = end1 - begin1;
                if(len1 < len2)
                    return end1;
                const I1 s = begin1 + (len2 - 1);  // End of pattern match can't go before here
                I1 l1 = end1;
                I2 l2 = end2;
                --l2;
                while(true)
                {
                    do if(s == l1) return end1;
                    while(!invoke(pred, invoke(proj, *--l1), *l2));
                    I1 m1 = l1;
                    I2 m2 = l2;
                    do  if(m2 == begin2) return m1;
                    // no need to check range on m1 because s guarantees we have enough source
                    while(invoke(pred, invoke(proj, *--m1), *--m2));
                }
            }

        public:
            template<typename I1, typename S1, typename I2, typename S2, typename R = equal_to,
                typename P = ident,
                CONCEPT_REQUIRES_(ForwardIterator<I1>() && Sentinel<S1, I1>() &&
                    ForwardIterator<I2>() && Sentinel<S2, I2>() &&
                    IndirectRelation<R, projected<I1, P>, I2>())>
            I1 operator()(I1 begin1, S1 end1, I2 begin2, S2 end2, R pred = R{}, P proj = P{}) const
            {
                constexpr bool Bidi = BidirectionalIterator<I1>() && BidirectionalIterator<I2>();
                return find_end_fn::impl(
                    begin1, detail::next_to_if<Bidi>(begin1, end1),
                    begin2, detail::next_to_if<Bidi>(begin2, end2),
                    std::move(pred), std::move(proj),
                    iterator_concept<I1>(), iterator_concept<I2>());
            }

            template<typename Rng1, typename Rng2, typename R = equal_to, typename P = ident,
                typename I1 = iterator_t<Rng1>,
                typename I2 = iterator_t<Rng2>,
                CONCEPT_REQUIRES_(ForwardRange<Rng1>() && ForwardRange<Rng2>() &&
                    IndirectRelation<R, projected<I1, P>, I2>())>
            safe_iterator_t<Rng1> operator()(Rng1 &&rng1, Rng2 &&rng2, R pred = R{}, P proj = P{}) const
            {
                return (*this)(begin(rng1), end(rng1), begin(rng2), end(rng2), std::move(pred),
                    std::move(proj));
            }
        };

        /// \sa `find_end_fn`
        /// \ingroup group-algorithms
        RANGES_INLINE_VARIABLE(with_braced_init_args<find_end_fn>, find_end)
        /// @}
    } // namespace v3
} // namespace ranges

#endif // include guard
