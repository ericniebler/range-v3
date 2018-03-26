/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2013-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
#ifndef RANGES_V3_ALGORITHM_PARTIAL_SORT_COPY_HPP
#define RANGES_V3_ALGORITHM_PARTIAL_SORT_COPY_HPP

#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/iterator.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/algorithm/heap_algorithm.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \ingroup group-concepts
        template<typename I, typename O, typename C = ordered_less, typename PI = ident,
            typename PO = ident>
        using PartialSortCopyConcept = meta::strict_and<
            InputIterator<I>,
            RandomAccessIterator<O>,
            IndirectlyCopyable<I, O>,
            IndirectRelation<C, projected<I, PI>, projected<O, PO>>,
            Sortable<O, C, PO>>;

        /// \addtogroup group-algorithms
        /// @{
        struct partial_sort_copy_fn
        {
            template<typename I, typename SI, typename O, typename SO, typename C = ordered_less,
                typename PI = ident, typename PO = ident,
                CONCEPT_REQUIRES_(PartialSortCopyConcept<I, O, C, PI, PO>() &&
                    Sentinel<SI, I>() && Sentinel<SO, O>())>
            O operator()(I begin, SI end, O out_begin, SO out_end, C pred = C{}, PI in_proj = PI{},
                PO out_proj = PO{}) const
            {
                O r = out_begin;
                if(r != out_end)
                {
                    for(; begin != end && r != out_end; ++begin, ++r)
                        *r = *begin;
                    make_heap(out_begin, r, std::ref(pred), std::ref(out_proj));
                    auto len = r - out_begin;
                    for(; begin != end; ++begin)
                    {
                        auto &&x = *begin;
                        if(invoke(pred, invoke(in_proj, x), invoke(out_proj, *out_begin)))
                        {
                            *out_begin = (decltype(x) &&) x;
                            detail::sift_down_n(out_begin, len, out_begin, std::ref(pred), std::ref(out_proj));
                        }
                    }
                    sort_heap(out_begin, r, std::ref(pred), std::ref(out_proj));
                }
                return r;
            }

            template<typename InRng, typename OutRng, typename C = ordered_less,
                typename PI = ident, typename PO = ident,
                typename I = iterator_t<InRng>,
                typename O = iterator_t<OutRng>,
                CONCEPT_REQUIRES_(PartialSortCopyConcept<I, O, C, PI, PO>() &&
                    Range<InRng>() && Range<OutRng>())>
            safe_iterator_t<OutRng>
            operator()(InRng && in_rng, OutRng &&out_rng, C pred = C{}, PI in_proj = PI{},
                PO out_proj = PO{}) const
            {
                return (*this)(begin(in_rng), end(in_rng), begin(out_rng), end(out_rng),
                    std::move(pred), std::move(in_proj), std::move(out_proj));
            }
        };

        /// \sa `partial_sort_copy_fn`
        /// \ingroup group-algorithms
        RANGES_INLINE_VARIABLE(with_braced_init_args<partial_sort_copy_fn>, partial_sort_copy)
        /// @}
    } // namespace v3
} // namespace ranges

#endif // include guard
