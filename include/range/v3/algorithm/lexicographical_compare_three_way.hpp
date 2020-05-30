/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2013-present
//  Copyright Google LLC 2020-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
#ifndef RANGES_V3_ALGORITHM_LEXICOGRAPHICAL_COMPARE_THREE_WAY_HPP
#define RANGES_V3_ALGORITHM_LEXICOGRAPHICAL_COMPARE_THREE_WAY_HPP

#if __cplusplus > 201703L
#include <version>

#if defined(__cpp_impl_three_way_comparison) && defined(__cpp_lib_three_way_comparison)

#include <range/v3/range_fwd.hpp>

#include <compare>

#include <range/v3/functional/identity.hpp>
#include <range/v3/functional/invoke.hpp>
#include <range/v3/iterator/concepts.hpp>
#include <range/v3/utility/static_const.hpp>

#include <range/v3/detail/disable_warnings.hpp>

namespace ranges
{
    namespace detail
    {
        template<typename F, typename I0, typename I1>
        concept indirectly_three_way_comparable_ =
            readable<I0> &&
            readable<I1> &&
            copy_constructible<F> &&
            indirectly_regular_binary_invocable_<F&, I0, I1> &&
            convertible_to<indirect_result_t<F&, I0, I1>, std::partial_ordering>;
    } // namespace detail

    /// \addtogroup group-algorithms
    /// @{
    RANGES_FUNC_BEGIN(lexicographical_compare_three_way)

        /// \brief function template \c lexicographical_compare_three_way
        template<input_iterator I0,
                 sentinel_for<I0> S0,
                 input_iterator I1,
                 sentinel_for<I1> S1,
                 typename C = std::compare_three_way,
                 typename P0 = identity,
                 typename P1 = identity>
        auto RANGES_FUNC(lexicographical_compare_three_way)(I0 begin0,
                                                            S0 end0,
                                                            I1 begin1,
                                                            S1 end1,
                                                            C comp = {},
                                                            P0 proj0 = P0{},
                                                            P1 proj1 = P1{})
            -> CPP_ret(indirect_result_t<C&, projected<I0, P0>, projected<I1, P1>>)(//
                 requires input_iterator<I0> && sentinel_for<S0, I0> &&
                 input_iterator<I1> && sentinel_for<S1, I1> &&
                 detail::indirectly_three_way_comparable_<C, projected<I0, P0>, projected<I1, P1>>)
        {
            for (; begin0 != end0 && begin1 != end1; (void)++begin0, (void)++begin1) {
                if (auto cmp = invoke(comp, invoke(proj0, *begin0), invoke(proj1, *begin1)); cmp != 0) {
                    return cmp;
                }
            }
            return begin0 != end0 ? std::strong_ordering::greater :
                   begin1 != end1 ? std::strong_ordering::less
                                  : std::strong_ordering::equal;
        }

        /// \overload
        template<typename R0,
                 typename R1,
                 typename C = std::compare_three_way,
                 typename P0 = identity,
                 typename P1 = identity>
         auto RANGES_FUNC(lexicographical_compare_three_way)(R0&& r0, R1&& r1, C comp = {}, P0 proj0 = {}, P1 proj1 = {}) //
             -> CPP_ret(indirect_result_t<C&, projected<iterator_t<R0>, P0>, projected<iterator_t<R1>, P1>>)( //
                  requires input_range<R0> && input_range<R1> &&
                  detail::indirectly_three_way_comparable_<C, projected<iterator_t<R0>, P0>, projected<iterator_t<R1>, P1>>)
         {
             return (*this)(begin(r0),
                            end(r0),
                            begin(r1),
                            end(r1),
                            std::move(comp),
                            std::move(proj0),
                            std::move(proj1));
         }

    RANGES_FUNC_END(lexicographical_compare_three_way)
    /// @}
} // namespace ranges

#include <range/v3/detail/reenable_warnings.hpp>

#endif // spaceship supported
#endif // __cplusplus
#endif // RANGES_V3_ALGORITHM_LEXICOGRAPHICAL_COMPARE_THREE_WAY_HPP
