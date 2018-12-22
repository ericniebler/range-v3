/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2014-present
//  Copyright Casey Carter 2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
#ifndef RANGES_V3_ALGORITHM_MINMAX_HPP
#define RANGES_V3_ALGORITHM_MINMAX_HPP

#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/iterator.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/algorithm/result_types.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-algorithms
        /// @{
        template<typename T>
        using minmax_result = detail::min_max_result<T, T>;

        struct minmax_fn
        {
            template<typename Rng, typename C = less, typename P = identity>
            constexpr /*c++14*/ auto operator()(Rng &&rng, C pred = C{}, P proj = P{}) const ->
                CPP_ret(minmax_result<iter_value_t<iterator_t<Rng>>>)(
                    requires InputRange<Rng> && Copyable<iter_value_t<iterator_t<Rng>>> &&
                        IndirectRelation<C, projected<iterator_t<Rng>, P>>)
            {
                using R = minmax_result<iter_value_t<iterator_t<Rng>>>;
                auto begin = ranges::begin(rng);
                auto end = ranges::end(rng);
                RANGES_EXPECT(begin != end);
                auto result = R{*begin, *begin};
                if(++begin != end)
                {
                    {
                        auto && tmp = *begin;
                        if(invoke(pred, invoke(proj, tmp), invoke(proj, result.min)))
                            result.min = (decltype(tmp) &&) tmp;
                        else
                            result.max = (decltype(tmp) &&) tmp;
                    }
                    while(++begin != end)
                    {
                        iter_value_t<iterator_t<Rng>> tmp1 = *begin;
                        if(++begin == end)
                        {
                            if(invoke(pred, invoke(proj, tmp1), invoke(proj, result.min)))
                                result.min = std::move(tmp1);
                            else if(!invoke(pred, invoke(proj, tmp1), invoke(proj, result.max)))
                                result.max = std::move(tmp1);
                            break;
                        }

                        auto && tmp2 = *begin;
                        if(invoke(pred, invoke(proj, tmp2), invoke(proj, tmp1)))
                        {
                            if(invoke(pred, invoke(proj, tmp2), invoke(proj, result.min)))
                                result.min = (decltype(tmp2) &&) tmp2;
                            if(!invoke(pred, invoke(proj, tmp1), invoke(proj, result.max)))
                                result.max = std::move(tmp1);
                        }
                        else
                        {
                            if(invoke(pred, invoke(proj, tmp1), invoke(proj, result.min)))
                                result.min = std::move(tmp1);
                            if(!invoke(pred, invoke(proj, tmp2), invoke(proj, result.max)))
                                result.max = (decltype(tmp2) &&) tmp2;
                        }
                    }
                }
                return result;
            }

            template<typename T, typename C = less, typename P = identity>
            constexpr auto operator()(T const &a, T const &b, C pred = C{}, P proj = P{}) const ->
                CPP_ret(minmax_result<T const &>)(
                    requires IndirectRelation<C, projected<const T *, P>>)
            {
                using R = minmax_result<T const &>;
                return invoke(pred, invoke(proj, b), invoke(proj, a)) ? R{b, a} : R{a, b};
            }
        };

        /// \sa `minmax_fn`
        /// \ingroup group-algorithms
        RANGES_INLINE_VARIABLE(with_braced_init_args<minmax_fn>, minmax)
        /// @}
    } // namespace v3
} // namespace ranges

#endif // include guard
