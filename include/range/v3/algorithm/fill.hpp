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
#ifndef RANGES_V3_ALGORITHM_FILL_HPP
#define RANGES_V3_ALGORITHM_FILL_HPP

#include <range/v3/range_fwd.hpp>

#include <range/v3/iterator/concepts.hpp>
#include <range/v3/range/access.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/range/dangling.hpp>
#include <range/v3/range/traits.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    /// \addtogroup group-algorithms
    /// @{
    RANGES_BEGIN_NIEBLOID(fill)

        /// \brief function template \c fill
        template<typename O, typename S, typename V>
        auto RANGES_FUN_NIEBLOID(fill)(O first, S last, V const & val) //
            ->CPP_ret(O)(                                              //
                requires output_iterator<O, V const &> && sentinel_for<S, O>)
        {
            for(; first != last; ++first)
                *first = val;
            return first;
        }

        /// \overload
        template<typename Rng, typename V>
        auto RANGES_FUN_NIEBLOID(fill)(Rng && rng, V const & val)
            ->CPP_ret(safe_iterator_t<Rng>)( //
                requires output_range<Rng, V const &>)
        {
            return (*this)(begin(rng), end(rng), val);
        }

    RANGES_END_NIEBLOID(fill)

    namespace cpp20
    {
        using ranges::fill;
    }
    /// @}
} // namespace ranges

#endif // include guard
