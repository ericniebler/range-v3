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
#ifndef RANGES_V3_ALGORITHM_REPLACE_COPY_HPP
#define RANGES_V3_ALGORITHM_REPLACE_COPY_HPP

#include <meta/meta.hpp>

#include <range/v3/range_fwd.hpp>

#include <range/v3/algorithm/result_types.hpp>
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
    template<typename I, typename O>
    using replace_copy_result = detail::in_out_result<I, O>;

    RANGES_BEGIN_NIEBLOID(replace_copy)

        /// \brief function template \c replace_copy
        template<typename I,
                 typename S,
                 typename O,
                 typename T1,
                 typename T2,
                 typename P = identity>
        auto RANGES_FUN_NIEBLOID(replace_copy)(I first,
                                               S last,
                                               O out,
                                               T1 const & old_value,
                                               T2 const & new_value,
                                               P proj = {}) //
            ->CPP_ret(replace_copy_result<I, O>)(           //
                requires input_iterator<I> && sentinel_for<S, I> &&
                output_iterator<O, T2 const &> && indirectly_copyable<I, O> &&
                indirect_relation<equal_to, projected<I, P>, T1 const *>)
        {
            for(; first != last; ++first, ++out)
            {
                auto && x = *first;
                if(invoke(proj, x) == old_value)
                    *out = new_value;
                else
                    *out = (decltype(x) &&)x;
            }
            return {first, out};
        }

        /// \overload
        template<typename Rng,
                 typename O,
                 typename T1,
                 typename T2,
                 typename P = identity>
        auto RANGES_FUN_NIEBLOID(replace_copy)(
            Rng && rng, O out, T1 const & old_value, T2 const & new_value, P proj = {}) //
            ->CPP_ret(replace_copy_result<safe_iterator_t<Rng>, O>)(                    //
                requires input_range<Rng> && output_iterator<O, T2 const &> &&
                indirectly_copyable<iterator_t<Rng>, O> &&
                indirect_relation<equal_to, projected<iterator_t<Rng>, P>, T1 const *>)
        {
            return (*this)(begin(rng),
                           end(rng),
                           std::move(out),
                           old_value,
                           new_value,
                           std::move(proj));
        }

    RANGES_END_NIEBLOID(replace_copy)

    namespace cpp20
    {
        using ranges::replace_copy;
        using ranges::replace_copy_result;
    } // namespace cpp20
    /// @}
} // namespace ranges

#endif // include guard
