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
#ifndef RANGES_V3_ALGORITHM_UNIQUE_COPY_HPP
#define RANGES_V3_ALGORITHM_UNIQUE_COPY_HPP

#include <meta/meta.hpp>

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

    template<typename I, typename O>
    using unique_copy_result = detail::in_out_result<I, O>;

    /// \cond
    namespace detail
    {
        template<typename I, typename S, typename O, typename C, typename P>
        unique_copy_result<I, O> unique_copy_impl(I first, S last, O out, C pred, P proj,
                                                  detail::input_iterator_tag_,
                                                  std::false_type)
        {
            if(first != last)
            {
                // Must save a copy into a local because we will need this value
                // even after we advance the input iterator.
                iter_value_t<I> value =
                    *first; // This is guaranteed by indirectly_copyable
                *out = value;
                ++out;
                while(++first != last)
                {
                    auto && x = *first;
                    if(!invoke(pred, invoke(proj, value), invoke(proj, x)))
                    {
                        value = (decltype(x) &&)x;
                        *out = value;
                        ++out;
                    }
                }
            }
            return {first, out};
        }

        template<typename I, typename S, typename O, typename C, typename P>
        unique_copy_result<I, O> unique_copy_impl(I first, S last, O out, C pred, P proj,
                                                  detail::forward_iterator_tag_,
                                                  std::false_type)
        {
            if(first != last)
            {
                I tmp = first;
                *out = *tmp;
                ++out;
                while(++first != last)
                {
                    auto && x = *first;
                    if(!invoke(pred, invoke(proj, *tmp), invoke(proj, x)))
                    {
                        *out = (decltype(x) &&)x;
                        ++out;
                        tmp = first;
                    }
                }
            }
            return {first, out};
        }

        template<typename I, typename S, typename O, typename C, typename P>
        unique_copy_result<I, O> unique_copy_impl(I first, S last, O out, C pred, P proj,
                                                  detail::input_iterator_tag_,
                                                  std::true_type)
        {
            if(first != last)
            {
                *out = *first;
                while(++first != last)
                {
                    auto && x = *first;
                    if(!invoke(pred, invoke(proj, *out), invoke(proj, x)))
                        *++out = (decltype(x) &&)x;
                }
                ++out;
            }
            return {first, out};
        }
    } // namespace detail
    /// \endcond

    RANGES_BEGIN_NIEBLOID(unique_copy)

        /// \brief template function unique_copy
        ///
        /// range-based version of the `unique_copy` std algorithm
        ///
        /// \pre `Rng` is a model of the `input_range` concept
        /// \pre `O` is a model of the `weakly_incrementable` concept
        /// \pre `C` is a model of the `relation` concept
        template<typename I,
                 typename S,
                 typename O,
                 typename C = equal_to,
                 typename P = identity>
        auto RANGES_FUN_NIEBLOID(unique_copy)(
            I first, S last, O out, C pred = C{}, P proj = P{}) //
            ->CPP_ret(unique_copy_result<I, O>)(                //
                requires input_iterator<I> && sentinel_for<S, I> &&
                indirect_relation<C, projected<I, P>> && weakly_incrementable<O> &&
                indirectly_copyable<I, O> &&
                (forward_iterator<I> || forward_iterator<O> ||
                 indirectly_copyable_storable<I, O>))
        {
            return detail::unique_copy_impl(std::move(first),
                                            std::move(last),
                                            std::move(out),
                                            std::move(pred),
                                            std::move(proj),
                                            iterator_tag_of<I>(),
                                            meta::bool_<forward_iterator<O>>{});
        }

        /// \overload
        template<typename Rng, typename O, typename C = equal_to, typename P = identity>
        auto RANGES_FUN_NIEBLOID(unique_copy)(
            Rng && rng, O out, C pred = C{}, P proj = P{})          //
            ->CPP_ret(unique_copy_result<safe_iterator_t<Rng>, O>)( //
                requires input_range<Rng> &&
                indirect_relation<C, projected<iterator_t<Rng>, P>> &&
                weakly_incrementable<O> && indirectly_copyable<iterator_t<Rng>, O> &&
                (forward_iterator<iterator_t<Rng>> || forward_iterator<O> ||
                 indirectly_copyable_storable<iterator_t<Rng>, O>))
        {
            return detail::unique_copy_impl(begin(rng),
                                            end(rng),
                                            std::move(out),
                                            std::move(pred),
                                            std::move(proj),
                                            iterator_tag_of<iterator_t<Rng>>(),
                                            meta::bool_<forward_iterator<O>>{});
        }

    RANGES_END_NIEBLOID(unique_copy)

    namespace cpp20
    {
        using ranges::unique_copy;
        using ranges::unique_copy_result;
    } // namespace cpp20
    /// @}
} // namespace ranges

#endif // include guard
