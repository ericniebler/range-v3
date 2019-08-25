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

#include <range/v3/algorithm/heap_algorithm.hpp>
#include <range/v3/functional/comparisons.hpp>
#include <range/v3/functional/identity.hpp>
#include <range/v3/functional/invoke.hpp>
#include <range/v3/iterator/concepts.hpp>
#include <range/v3/iterator/traits.hpp>
#include <range/v3/range/access.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/range/dangling.hpp>
#include <range/v3/range/traits.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    /// \addtogroup group-algorithms
    /// @{
    RANGES_BEGIN_NIEBLOID(partial_sort_copy)

        /// \brief function template \c partial_sort_copy
        template<typename I,
                 typename SI,
                 typename O,
                 typename SO,
                 typename C = less,
                 typename PI = identity,
                 typename PO = identity>
        auto RANGES_FUN_NIEBLOID(partial_sort_copy)(I first,
                                                    SI last,
                                                    O out_begin,
                                                    SO out_end,
                                                    C pred = C{},
                                                    PI in_proj = PI{},
                                                    PO out_proj = PO{}) //
            ->CPP_ret(O)(                                               //
                requires input_iterator<I> && sentinel_for<SI, I> &&
                random_access_iterator<O> && sentinel_for<SO, O> &&
                indirectly_copyable<I, O> && sortable<O, C, PO> &&
                indirect_strict_weak_order<C, projected<I, PI>, projected<O, PO>>)
        {
            O r = out_begin;
            if(r != out_end)
            {
                for(; first != last && r != out_end; ++first, ++r)
                    *r = *first;
                make_heap(out_begin, r, std::ref(pred), std::ref(out_proj));
                auto len = r - out_begin;
                for(; first != last; ++first)
                {
                    auto && x = *first;
                    if(invoke(pred, invoke(in_proj, x), invoke(out_proj, *out_begin)))
                    {
                        *out_begin = (decltype(x) &&)x;
                        detail::sift_down_n(out_begin,
                                            len,
                                            out_begin,
                                            std::ref(pred),
                                            std::ref(out_proj));
                    }
                }
                sort_heap(out_begin, r, std::ref(pred), std::ref(out_proj));
            }
            return r;
        }

        /// \overload
        template<typename InRng,
                 typename OutRng,
                 typename C = less,
                 typename PI = identity,
                 typename PO = identity>
        auto RANGES_FUN_NIEBLOID(partial_sort_copy)(InRng && in_rng,
                                                    OutRng && out_rng,
                                                    C pred = C{},
                                                    PI in_proj = PI{},
                                                    PO out_proj = PO{}) //
            ->CPP_ret(safe_iterator_t<OutRng>)(                         //
                requires input_range<InRng> && random_access_range<OutRng> &&
                indirectly_copyable<iterator_t<InRng>, iterator_t<OutRng>> &&
                sortable<iterator_t<OutRng>, C, PO> &&
                indirect_strict_weak_order<C,
                                           projected<iterator_t<InRng>, PI>,
                                           projected<iterator_t<OutRng>, PO>>)
        {
            return (*this)(begin(in_rng),
                           end(in_rng),
                           begin(out_rng),
                           end(out_rng),
                           std::move(pred),
                           std::move(in_proj),
                           std::move(out_proj));
        }

    RANGES_END_NIEBLOID(partial_sort_copy)

    namespace cpp20
    {
        using ranges::partial_sort_copy;
    }
    /// @}
} // namespace ranges

#endif // include guard
