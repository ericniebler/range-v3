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
// Implementation based on the code in libc++
//   http://http://libcxx.llvm.org/

#ifndef RANGES_V3_ALGORITHM_MINMAX_ELEMENT_HPP
#define RANGES_V3_ALGORITHM_MINMAX_ELEMENT_HPP

#include <range/v3/range_fwd.hpp>

#include <range/v3/algorithm/result_types.hpp>
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
    template<typename I>
    using minmax_element_result = detail::min_max_result<I, I>;

    RANGES_BEGIN_NIEBLOID(minmax_element)

        /// \brief function template \c minmax_element
        template<typename I, typename S, typename C = less, typename P = identity>
        auto RANGES_FUN_NIEBLOID(minmax_element)(
            I first, S last, C pred = C{}, P proj = P{}) //
            ->CPP_ret(minmax_element_result<I>)(         //
                requires forward_iterator<I> && sentinel_for<S, I> &&
                indirect_strict_weak_order<C, projected<I, P>>)
        {
            minmax_element_result<I> result{first, first};
            if(first == last || ++first == last)
                return result;
            if(invoke(pred, invoke(proj, *first), invoke(proj, *result.min)))
                result.min = first;
            else
                result.max = first;
            while(++first != last)
            {
                I tmp = first;
                if(++first == last)
                {
                    if(invoke(pred, invoke(proj, *tmp), invoke(proj, *result.min)))
                        result.min = tmp;
                    else if(!invoke(pred, invoke(proj, *tmp), invoke(proj, *result.max)))
                        result.max = tmp;
                    break;
                }
                else
                {
                    if(invoke(pred, invoke(proj, *first), invoke(proj, *tmp)))
                    {
                        if(invoke(pred, invoke(proj, *first), invoke(proj, *result.min)))
                            result.min = first;
                        if(!invoke(pred, invoke(proj, *tmp), invoke(proj, *result.max)))
                            result.max = tmp;
                    }
                    else
                    {
                        if(invoke(pred, invoke(proj, *tmp), invoke(proj, *result.min)))
                            result.min = tmp;
                        if(!invoke(pred, invoke(proj, *first), invoke(proj, *result.max)))
                            result.max = first;
                    }
                }
            }
            return result;
        }

        /// \overload
        template<typename Rng, typename C = less, typename P = identity>
        auto RANGES_FUN_NIEBLOID(minmax_element)(
            Rng && rng, C pred = C{}, P proj = P{})                 //
            ->CPP_ret(minmax_element_result<safe_iterator_t<Rng>>)( //
                requires forward_range<Rng> &&
                indirect_strict_weak_order<C, projected<iterator_t<Rng>, P>>)
        {
            return (*this)(begin(rng), end(rng), std::move(pred), std::move(proj));
        }

    RANGES_END_NIEBLOID(minmax_element)

    namespace cpp20
    {
        using ranges::minmax_element;
        using ranges::minmax_element_result;
    } // namespace cpp20
    /// @}
} // namespace ranges

#endif // include guard
