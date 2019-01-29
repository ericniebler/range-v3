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
#include <range/v3/range/access.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/range/dangling.hpp>
#include <range/v3/range/traits.hpp>
#include <range/v3/algorithm/result_types.hpp>
#include <range/v3/functional/comparisons.hpp>
#include <range/v3/functional/identity.hpp>
#include <range/v3/functional/invoke.hpp>
#include <range/v3/iterator/concepts.hpp>
#include <range/v3/iterator/traits.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    /// \addtogroup group-algorithms
    /// @{
    template<typename I>
    using minmax_element_result = detail::min_max_result<I, I>;

    struct minmax_element_fn
    {
        template<typename I, typename S, typename C = less, typename P = identity>
        auto operator()(I begin, S end, C pred = C{}, P proj = P{}) const ->
            CPP_ret(minmax_element_result<I>)(
                requires ForwardIterator<I> && Sentinel<S, I> &&
                    IndirectStrictWeakOrder<C, projected<I, P>>)
        {
            minmax_element_result<I> result{begin, begin};
            if(begin == end || ++begin == end)
                return result;
            if(invoke(pred, invoke(proj, *begin), invoke(proj, *result.min)))
                result.min = begin;
            else
                result.max = begin;
            while(++begin != end)
            {
                I tmp = begin;
                if(++begin == end)
                {
                    if(invoke(pred, invoke(proj, *tmp), invoke(proj, *result.min)))
                        result.min = tmp;
                    else if(!invoke(pred, invoke(proj, *tmp), invoke(proj, *result.max)))
                        result.max = tmp;
                    break;
                }
                else
                {
                    if(invoke(pred, invoke(proj, *begin), invoke(proj, *tmp)))
                    {
                        if(invoke(pred, invoke(proj, *begin), invoke(proj, *result.min)))
                            result.min = begin;
                        if(!invoke(pred, invoke(proj, *tmp), invoke(proj, *result.max)))
                            result.max = tmp;
                    }
                    else
                    {
                        if(invoke(pred, invoke(proj, *tmp), invoke(proj, *result.min)))
                            result.min = tmp;
                        if(!invoke(pred, invoke(proj, *begin), invoke(proj, *result.max)))
                            result.max = begin;
                    }
                }
            }
            return result;
        }

        template<typename Rng, typename C = less, typename P = identity>
        auto operator()(Rng &&rng, C pred = C{}, P proj = P{}) const ->
            CPP_ret(minmax_element_result<safe_iterator_t<Rng>>)(
                requires ForwardRange<Rng> &&
                    IndirectStrictWeakOrder<C, projected<iterator_t<Rng>, P>>)
        {
            return (*this)(begin(rng), end(rng), std::move(pred), std::move(proj));
        }
    };

    /// \sa `minmax_element_fn`
    /// \ingroup group-algorithms
    RANGES_INLINE_VARIABLE(minmax_element_fn, minmax_element)
    /// @}
} // namespace ranges

#endif // include guard
