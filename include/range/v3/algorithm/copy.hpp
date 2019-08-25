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
#ifndef RANGES_V3_ALGORITHM_COPY_HPP
#define RANGES_V3_ALGORITHM_COPY_HPP

#include <functional>
#include <utility>

#include <range/v3/range_fwd.hpp>

#include <range/v3/algorithm/result_types.hpp>
#include <range/v3/iterator/concepts.hpp>
#include <range/v3/iterator/traits.hpp>
#include <range/v3/range/access.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/range/dangling.hpp>
#include <range/v3/range/traits.hpp>
#include <range/v3/utility/copy.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    /// \addtogroup group-algorithms
    /// @{
    template<typename I, typename O>
    using copy_result = detail::in_out_result<I, O>;

    RANGES_HIDDEN_DETAIL(namespace _copy CPP_PP_LBRACE())
    RANGES_BEGIN_NIEBLOID(copy)

        /// \brief function template \c copy
        template<typename I, typename S, typename O>
        constexpr auto RANGES_FUN_NIEBLOID(copy)(I first, S last, O out) //
            ->CPP_ret(copy_result<I, O>)(                                //
                requires input_iterator<I> && sentinel_for<S, I> &&
                weakly_incrementable<O> && indirectly_copyable<I, O>)
        {
            for(; first != last; ++first, ++out)
                *out = *first;
            return {first, out};
        } // namespace ranges

        /// \overload
        template<typename Rng, typename O>
        constexpr auto RANGES_FUN_NIEBLOID(copy)(Rng && rng, O out) //
            ->CPP_ret(copy_result<safe_iterator_t<Rng>, O>)(        //
                requires input_range<Rng> && weakly_incrementable<O> &&
                indirectly_copyable<iterator_t<Rng>, O>)
        {
            return (*this)(begin(rng), end(rng), std::move(out));
        }

    RANGES_END_NIEBLOID(copy)
    RANGES_HIDDEN_DETAIL(CPP_PP_RBRACE())

#ifndef RANGES_DOXYGEN_INVOKED
    struct copy_fn
      : aux::copy_fn
      , _copy::copy_fn
    {
        using aux::copy_fn::operator();
        using _copy::copy_fn::operator();
    };
    RANGES_INLINE_VARIABLE(copy_fn, copy)
#endif

    namespace cpp20
    {
        using ranges::copy_result;
        using ranges::RANGES_HIDDEN_DETAIL(_copy::) copy;
    } // namespace cpp20

    /// @}
} // namespace ranges

#endif // include guard
