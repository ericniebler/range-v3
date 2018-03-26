/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2014-present
//  Copyright Casey Carter 2016
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
#ifndef RANGES_V3_ALGORITHM_AUX_LOWER_BOUND_N_HPP
#define RANGES_V3_ALGORITHM_AUX_LOWER_BOUND_N_HPP

#include <range/v3/range_fwd.hpp>
#include <range/v3/algorithm/aux_/partition_point_n.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \cond
        namespace detail
        {
            // [&](auto&& i){ return invoke(pred, i, val); }
            template<typename Pred, typename Val>
            struct lower_bound_predicate
            {
                Pred& pred_;
                Val& val_;

                template<typename T>
                bool operator()(T&& t) const
                {
                    return invoke(pred_, static_cast<T&&>(t), val_);
                }
            };

            template<typename Pred, typename Val>
            lower_bound_predicate<Pred, Val>
            make_lower_bound_predicate(Pred& pred, Val& val)
            {
                return {pred, val};
            }
        }
        /// \endcond

        namespace aux
        {
            struct lower_bound_n_fn
            {
                template<typename I, typename V2, typename C = ordered_less, typename P = ident,
                    CONCEPT_REQUIRES_(BinarySearchable<I, V2, C, P>())>
                I operator()(I begin, difference_type_t<I> d, V2 const &val, C pred = C{},
                    P proj = P{}) const
                {
                    return partition_point_n(std::move(begin), d,
                        detail::make_lower_bound_predicate(pred, val), std::move(proj));
                }
            };

            RANGES_INLINE_VARIABLE(lower_bound_n_fn, lower_bound_n)
        }
    } // namespace v3
} // namespace ranges

#endif // include guard
