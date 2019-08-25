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
#ifndef RANGES_V3_ALGORITHM_MIN_HPP
#define RANGES_V3_ALGORITHM_MIN_HPP

#include <initializer_list>

#include <range/v3/range_fwd.hpp>

#include <range/v3/functional/comparisons.hpp>
#include <range/v3/functional/identity.hpp>
#include <range/v3/functional/invoke.hpp>
#include <range/v3/iterator/concepts.hpp>
#include <range/v3/iterator/traits.hpp>
#include <range/v3/range/access.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/range/traits.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    /// \addtogroup group-algorithms
    /// @{
    RANGES_BEGIN_NIEBLOID(min)

        /// \brief function template \c min
        template<typename T, typename C = less, typename P = identity>
        constexpr auto RANGES_FUN_NIEBLOID(min)(
            T const & a, T const & b, C pred = C{}, P proj = P{}) //
            ->CPP_ret(T const &)(                                 //
                requires indirect_strict_weak_order<C, projected<T const *, P>>)
        {
            return invoke(pred, invoke(proj, b), invoke(proj, a)) ? b : a;
        }

        /// \overload
        template<typename Rng, typename C = less, typename P = identity>
        constexpr auto RANGES_FUN_NIEBLOID(min)(Rng && rng, C pred = C{}, P proj = P{}) //
            ->CPP_ret(range_value_t<Rng>)(                                              //
                requires input_range<Rng> &&
                indirect_strict_weak_order<C, projected<iterator_t<Rng>, P>> &&
                indirectly_copyable_storable<iterator_t<Rng>, range_value_t<Rng> *>)
        {
            auto first = ranges::begin(rng);
            auto last = ranges::end(rng);
            RANGES_EXPECT(first != last);
            range_value_t<Rng> result = *first;
            while(++first != last)
            {
                auto && tmp = *first;
                if(invoke(pred, invoke(proj, tmp), invoke(proj, result)))
                    result = (decltype(tmp) &&)tmp;
            }
            return result;
        }

        /// \overload
        template<typename T, typename C = less, typename P = identity>
        constexpr auto RANGES_FUN_NIEBLOID(min)(
            std::initializer_list<T> const && rng, C pred = C{}, P proj = P{}) //
            ->CPP_ret(T)(                                                      //
                requires copyable<T> &&
                indirect_strict_weak_order<C, projected<T const *, P>>)
        {
            return (*this)(rng, std::move(pred), std::move(proj));
        }

    RANGES_END_NIEBLOID(min)

    namespace cpp20
    {
        using ranges::min;
    }
    /// @}
} // namespace ranges

#endif // include guard
